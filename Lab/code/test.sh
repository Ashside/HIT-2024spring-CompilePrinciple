make clean
# 如果第一个参数是d，那么运行make debug
if [ $2 = "d" ]; then
	make Lab2DBG
else
	make
fi
# 读入一个参数
echo "------------------------------------------------------"
echo "Running Testcase $1"
./parser ../testcases/input$1.cmm
echo "------------------------------------------------------"
make clean