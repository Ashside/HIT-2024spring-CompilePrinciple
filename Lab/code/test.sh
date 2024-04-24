make clean
make
# 读入一个参数
echo "------------------------------------------------------"
echo "Running Testcase $1"
./parser ../testcases/input$1.cmm
echo "------------------------------------------------------"
make clean