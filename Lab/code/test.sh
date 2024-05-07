make clean
# 如果第一个参数是d，那么运行make debug
if [ $1 = "d" ]; then
	make Lab3DBG
	# 读入一个参数
	echo "------------------------------------------------------"
	echo "Running Testcase $2"
	./parser ../testcases/input$2.cmm
	echo "------------------------------------------------------"
else
	make
	# 读入一个参数
	echo "------------------------------------------------------"
	echo "Running Testcase $1"
	./parser ../testcases/input$1.cmm > ../outputres/output$1.ir
	echo "------------------------------------------------------"
fi

make clean