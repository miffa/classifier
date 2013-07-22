#include "YsClassifierManager.h"
#include "YsTools.h"
#include "YsAdapter.h"
#include "logglobal.h"

//test
#include "YsTemplateHashData.h"
#include "YsHashData.h"
#include "YsDecisionTree.h"
	
int main(int argc, char** argv)
{
	g_traceLevel = 3; //设置日志级别
	strcpy(g_fileLine, "yss_clasifier");
	
	mysignal(SIGINT, SIG_IGN);
	mysignal(SIGHUP, SIG_IGN);
	
	//log init
	string logPath = getenv("HOME");
	logPath = logPath + "/tools/trunk/log/classifier.log";
	INF_LOG3("log path: %s\n", logPath.c_str());
	Ys_LogBaseInfo::getInstance()->setLogFileName(logPath);

	//Init
	if(!YS_ConfigData::GetInstance()->StartUp())
	{
		ERR_LOG("configuration is error, exit\n");
		exit(0);
	}
	
	g_traceLevel = YS_ConfigData::GetInstance()->m_logLevel;
	YS_ConfigData::GetInstance()->printInfo();
	Ys_BadCharacter::GetInstance()->StartUp();
	if(!Ys_ClassifierManager::GetInstance()->StartUp())
	{
		ERR_LOG("Ys_ClassifierManager starting is error, exit\n");
		exit(0);
	}
	INF_LOG("Ys_ClassifierManager init ok\n");
	
	//初始化白名单分类器
	Ys_WhiteAdapter* whiteClassifer = new Ys_WhiteAdapter();
	if(!whiteClassifer->StartUp())
		exit(0);
	//初始化白名单分类器
	Ys_TagWhiteAdapter* tagWhiteClassifer = new Ys_TagWhiteAdapter();
	if(!tagWhiteClassifer->StartUp())
		exit(0);
	//后缀树分类
	Ys_TireTreeAdapter* triedTreeClassifer = new Ys_TireTreeAdapter();
	if(!triedTreeClassifer->StartUp())
		exit(0);	
	//初始化贝叶斯分类器
	Ys_BayesAdapter*  bayesClassifier = new Ys_BayesAdapter();
	if(!bayesClassifier->StartUp())
		exit(0);

	//初始化决策树分类器
	/*Ys_DecsionTreeAdapter*  decisionTreeClassifier = new Ys_DecsionTreeAdapter();
	if(!decisionTreeClassifier->StartUp())
	{
		ERR_LOG("Ys_DecsionTreeAdapter starting is error, exit\n");
		exit(0);
	}*/
	
	Ys_ClassifierManager::GetInstance()->ProcessDataFile();
	/*
	//loop
	g_enterMainLoop = 0;
	for(;;)
	{
		//Ys_ClassifierManager::GetInstance()->ProcessDataFile();
	}
	*/
	return 0;
}
