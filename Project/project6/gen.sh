#!/bin/bash

# 批量生成多组纹理测试数据脚本
# 适配：DataGen.c中fopen路径为./dataset/UI/UI_textures.txt
# 修复问题：每次循环从备份恢复原始文件，避免替换后原字符串丢失

# ===================== 配置项（用户可修改）=====================
BATCH_COUNT=10          # 要生成的数据集数量（比如5组：UI_textures1~5.txt）
SRC_FILE="DataGen.c"   # 源文件名称
EXE_FILE="gen.exe"     # 编译后的可执行文件名称
BACKUP_FILE="${SRC_FILE}.bak" # 源文件备份名（保留原始内容）
# 新增：原文件的基础路径和文件名（关键配置）
BASE_DIR="./dataset/mix"          # 基础目录
BASE_FILENAME="mix_textures"      # 基础文件名（无后缀）
BASE_EXT="txt"                   # 文件后缀

# ===================== 颜色与日志配置 =====================
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # 重置颜色

# 打印带时间的日志
log() {
    echo -e "[$(date +%H:%M:%S)] $1"
}

# ===================== 前置检查（新增目录创建）=====================
# 1. 检查gcc是否安装
log "${YELLOW}【前置检查】验证gcc编译器...${NC}"
if ! command -v gcc &> /dev/null; then
    log "${RED}错误：未找到gcc编译器，请先安装gcc/MinGW！${NC}"
    exit 1
fi
log "${GREEN}✓ gcc已安装${NC}"

# 2. 检查源文件是否存在
if [ ! -f "${SRC_FILE}" ]; then
    log "${RED}错误：未找到源文件 ${SRC_FILE}，请确认脚本与源文件同目录！${NC}"
    exit 1
fi
log "${GREEN}✓ 找到源文件 ${SRC_FILE}${NC}"

# 3. 新增：创建基础目录（./dataset/UI），如果不存在
log "${YELLOW}【前置检查】创建目录 ${BASE_DIR}（若不存在）...${NC}"
mkdir -p "${BASE_DIR}"
if [ $? -ne 0 ]; then
    log "${RED}错误：创建目录 ${BASE_DIR} 失败！${NC}"
    exit 1
fi
log "${GREEN}✓ 目录 ${BASE_DIR} 已准备好${NC}"

# 4. 备份原文件（保留原始内容，整个脚本只备份一次）
log "${YELLOW}【备份】创建源文件备份 ${BACKUP_FILE}...${NC}"
cp -f "${SRC_FILE}" "${BACKUP_FILE}"
if [ $? -ne 0 ]; then
    log "${RED}错误：备份源文件失败！${NC}"
    exit 1
fi
log "${GREEN}✓ 源文件备份完成（保留原始内容）${NC}"

# ===================== 批量生成核心逻辑（关键修改：每次循环恢复原始文件）=====================
log "${YELLOW}【开始批量生成】共生成 ${BATCH_COUNT} 组数据...${NC}"
for ((i=1; i<=BATCH_COUNT; i++)); do
    # 目标输出文件名：./dataset/UI/UI_textures{i}.txt
    TARGET_FILENAME="${BASE_FILENAME}${i}.${BASE_EXT}"
    TARGET_FILE="${BASE_DIR}/${TARGET_FILENAME}"
    # 原文件中的路径：./dataset/UI/UI_textures.txt
    ORIGINAL_FILE="${BASE_DIR}/${BASE_FILENAME}.${BASE_EXT}"

    log "\n${YELLOW}===== 第 ${i}/${BATCH_COUNT} 组：生成 ${TARGET_FILE} =====${NC}"

    # ========== 关键修改1：每次循环前，从备份文件恢复原始的DataGen.c ==========
    log "${YELLOW}[0/${i}] 恢复原始 ${SRC_FILE} 文件...${NC}"
    cp -f "${BACKUP_FILE}" "${SRC_FILE}"
    if [ $? -ne 0 ]; then
        log "${RED}错误：恢复原始文件失败！${NC}"
        exit 1
    fi
    log "${GREEN}✓ 原始文件恢复完成${NC}"

    # 步骤1：替换DataGen.c中的输出文件名（sed分隔符为|，避免/冲突）
    log "${YELLOW}[1/${i}] 修改输出文件名为 ${TARGET_FILE}...${NC}"
    if [[ "$(uname)" == "Darwin" ]]; then
        # macOS系统：sed -i ''
        sed -i '' "s|fopen(\"${ORIGINAL_FILE}\", \"w\")|fopen(\"${TARGET_FILE}\", \"w\")|g" "${SRC_FILE}"
    else
        # Linux/Windows MinGW：sed -i
        sed -i "s|fopen(\"${ORIGINAL_FILE}\", \"w\")|fopen(\"${TARGET_FILE}\", \"w\")|g" "${SRC_FILE}"
    fi

    # 检查替换是否成功
    if ! grep -q "fopen(\"${TARGET_FILE}\", \"w\")" "${SRC_FILE}"; then
        log "${RED}错误：修改输出文件名失败！请检查 ${SRC_FILE} 中是否有 fopen(\"${ORIGINAL_FILE}\", \"w\")${NC}"
        # 恢复原文件并退出
        cp -f "${BACKUP_FILE}" "${SRC_FILE}"
        rm -f "${BACKUP_FILE}"
        exit 1
    fi
    log "${GREEN}✓ 文件名修改完成${NC}"

    # 步骤2：编译源文件
    log "${YELLOW}[2/${i}] 编译 ${SRC_FILE}...${NC}"
    gcc "${SRC_FILE}" -o "${EXE_FILE}" -Wall
    if [ $? -ne 0 ]; then
        log "${RED}错误：编译失败！${NC}"
        # 恢复原文件并退出
        cp -f "${BACKUP_FILE}" "${SRC_FILE}"
        rm -f "${BACKUP_FILE}"
        exit 1
    fi
    log "${GREEN}✓ 编译成功，生成 ${EXE_FILE}${NC}"

    # 步骤3：执行可执行文件生成数据
    log "${YELLOW}[3/${i}] 执行 ${EXE_FILE} 生成数据...${NC}"
    chmod +x "${EXE_FILE}"
    ./"${EXE_FILE}"

    # 检查数据文件是否生成
    if [ -f "${TARGET_FILE}" ]; then
        log "${GREEN}✓ 第 ${i} 组数据已生成：${TARGET_FILE}${NC}"
    else
        log "${RED}错误：执行 ${EXE_FILE} 后未生成 ${TARGET_FILE}！${NC}"
        # 恢复原文件并退出
        cp -f "${BACKUP_FILE}" "${SRC_FILE}"
        rm -f "${BACKUP_FILE}"
        exit 1
    fi
done

# ===================== 收尾工作 =====================
# 恢复原DataGen.c文件（最终恢复）
log "\n${YELLOW}【收尾】恢复原 ${SRC_FILE} 文件...${NC}"
cp -f "${BACKUP_FILE}" "${SRC_FILE}"
rm -f "${BACKUP_FILE}" # 删除备份文件
log "${GREEN}✓ 原文件已恢复，备份文件已删除${NC}"

# 清理可执行文件（可选，注释掉则保留）
rm -f "${EXE_FILE}"
log "${GREEN}✓ 可执行文件 ${EXE_FILE} 已清理${NC}"

# 最终提示
log "\n${GREEN}🎉 批量生成完成！共生成 ${BATCH_COUNT} 组数据：${BASE_DIR}/${BASE_FILENAME}1.${BASE_EXT} ~ ${BASE_DIR}/${BASE_FILENAME}${BATCH_COUNT}.${BASE_EXT}${NC}"

exit 0