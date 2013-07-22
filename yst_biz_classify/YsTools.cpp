#include "YsTools.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "YssCommon.h"
#include "YsWordSegmentLR.h"

YS_FileProcessor::YS_FileProcessor(const char* filename): m_strFileName(filename), m_filePtr(NULL),m_lFileLen(-1)
{
	
}

YS_FileProcessor::~YS_FileProcessor()
{
	CloseFile(); 
}
	
bool YS_FileProcessor::OpenForRead()
{
	m_filePtr = fopen(m_strFileName.c_str(), "r");
	if(m_filePtr == NULL)
		return false;
	SetFileLen();
	return true;
}

bool YS_FileProcessor::OpenForWrite()
{
	m_filePtr = fopen(m_strFileName.c_str(), "w");
	if(m_filePtr == NULL)
		return false;
	return true;
}

bool YS_FileProcessor::OpenForAppend()
{
	m_filePtr = fopen(m_strFileName.c_str(), "a");
	if(m_filePtr == NULL)
		return false;
	return true;
}
	
bool YS_FileProcessor::CloseFile()
{
	if(m_filePtr != NULL)
	{
		fclose(m_filePtr);
		m_filePtr = NULL;
	}
	return true;
}
	
bool YS_FileProcessor::SeekByIndex(long index)
{
	if( fseek(m_filePtr, index, SEEK_SET) < 0 )
		return false;
	return true;
}

int YS_FileProcessor::GetIntFromJavaToC(int data)
{
	long iRetStat = 0;
	char* cPtr = (char*)&iRetStat;
	cPtr[3] = (char) (data & 0xff);//java位置5  对应C++ 位置4
	cPtr[2] = (char) (data >> 8 & 0xff);//java位置6  对应C++ 位置3
	cPtr[1] = (char) (data >> 16 & 0xff);//java位置7  对应C++ 位置2
	cPtr[0] = (char) (data >> 24 & 0xff);//java位置8  对应C++ 位置1
	
	return iRetStat;
}

int YS_FileProcessor::ReadInt()
{
	int iRetValue = 0;
	int iRetStat = fread((void*)&iRetValue, JAVA_INT_BIT, ONE_TIME, m_filePtr);
	if( iRetStat != ONE_TIME)
		return -1;
	return GetIntFromJavaToC(iRetValue);
}

long YS_FileProcessor::GetLongFromJavaToC(long data)
{
	long iRetStat = 0;
	char* cPtr = (char*)&iRetStat;
	cPtr[7] = (char) (data & 0xff);      //java位置1  对应C++ 位置8
	cPtr[6] = (char) (data >> 8 & 0xff); //java位置2  对应C++ 位置7
	cPtr[5] = (char) (data >> 16 & 0xff);//java位置3  对应C++ 位置6
	cPtr[4] = (char) (data >> 24 & 0xff);//java位置4  对应C++ 位置5
	cPtr[3] = (char) (data >> 32 & 0xff);//java位置5  对应C++ 位置4
	cPtr[2] = (char) (data >> 40 & 0xff);//java位置6  对应C++ 位置3
	cPtr[1] = (char) (data >> 48 & 0xff);//java位置7  对应C++ 位置2
	cPtr[0] = (char) (data >> 56 & 0xff);//java位置8  对应C++ 位置1
	
	return iRetStat;
}

long YS_FileProcessor::ReadLong()
{
	long iRetValue;
	int iRetStat = fread((void*)&iRetValue, JAVA_LONG_BIT, ONE_TIME, m_filePtr);
	if( iRetStat != ONE_TIME)
		return -1;
	return GetLongFromJavaToC(iRetValue);
}

bool YS_FileProcessor::ReadData(void* szBuf, long size) {
	long iLeft = size;
	long iReadNum;
	long iRet;

	memset(szBuf, 0x00, size);
	char* bufBegin = (char*)szBuf;

	while (iLeft > 0) {
		
		iReadNum = iLeft > 2048 ? 2048 : iLeft;
		iRet = fread((char*)bufBegin, 1, iReadNum, m_filePtr);
		bufBegin += iReadNum;
		if (iRet != iReadNum) {
			if (!feof(m_filePtr))
				return false;
			return true;
		}
		iLeft -= iRet;
	}
	return true;
}

bool YS_FileProcessor::ReadLine(char* szBuf, long uiBufLen) {
    szBuf[0] = '\0';
	if (fgets(szBuf, uiBufLen, m_filePtr) == NULL) {
		if (!feof(m_filePtr))
			return false;
		return true;
	}
	if (szBuf[strlen(szBuf) - 1] == ' ' || szBuf[strlen(szBuf) - 1]== '\n')
		szBuf[strlen(szBuf) - 1] = 0x00;
	return true;
}

bool YS_FileProcessor::ReadByteArray(void* dest, long size)
{
	return ONE_TIME==fread(dest, size, ONE_TIME, m_filePtr);	
}

bool YS_FileProcessor::ReadCharArray(char* dest, long size)
{
	return size==fread(dest, sizeof(char), size, m_filePtr);
}

bool YS_FileProcessor::WriteLine(const char* buff)
{
	int ret0 = strlen(buff);
	int ret1 = fprintf(m_filePtr,"%s\n",  buff);
	return ret0+1 == ret1;
}

bool YS_FileProcessor::WriteBin(const void* buff, long size)
{
	return ONE_TIME==fwrite(buff, size, ONE_TIME, m_filePtr);
}

long YS_FileProcessor::SetFileLen()
{
	struct stat st;
	if(fstat(fileno(m_filePtr), &st) < 0 )
		return -1;
	m_lFileLen = st.st_size;
	return 0;	
}

bool YS_FileProcessor::IsEnd()
{
	if(feof(m_filePtr))
		return true;
	return false;
}

int YS_FileProcessor::GetFileFd()
{
	return fileno(m_filePtr);
}


YS_ConfigData* YS_ConfigData::m_pInstance = NULL;
YS_ConfigData* YS_ConfigData::GetInstance() 
{
	if(m_pInstance == NULL)
		m_pInstance = new YS_ConfigData();
	return m_pInstance;
} 

YS_ConfigData::YS_ConfigData()
{
	m_strSourcePath = "";   // data source
    m_bIfTrain = false;
	m_strTrainPath = "";    // training data source
	m_strTestPath = "";    //  test data source
	m_strWhiteList = "";   //  white list
	m_strCollfictList = "";  //collfict
	m_strDictPath = "";
	m_strGoodDataPath = "";
	m_strBadDataPath = "";
	m_strBakPath = "";
	m_bIfWordSeg = true;
	m_attributeKey = "name";
	m_logLevel = 2;
	m_strFeatureScore = "";
	m_attributeTitle.clear();
}
	
bool YS_ConfigData::StartUp()
{
	string strEtcPath = string(getenv("HOME")) + "/tools/trunk/etc/conf.classifer";
	FILE* filePtr = fopen(strEtcPath.c_str(), "r");
	if(filePtr == NULL)
	{
		logInfo(">>>>>>>>    open configuration error : %s ", strEtcPath.c_str());
		return false;
	}
	char buffer[TMP_BUFFER_LEN];
	memset(buffer, 0x00, TMP_BUFFER_LEN);
	while( fgets(buffer, TMP_BUFFER_LEN,filePtr)!= NULL )
	{
		string strItem(buffer);
		string strName("");
		string strValue("");
		string::size_type iPos = strItem.find( ":" );
		if ( iPos == string::npos )
			continue;
		string::size_type iPos_ = strItem.find( "|" );
		if ( iPos_ == string::npos )
			continue;
		else
		{
			strName =  strItem.substr(0,iPos);
			strValue =  strItem.substr(iPos+1, iPos_-iPos-1);
		}
		
        if(strName == "class_probability")
            m_strClassProbabilityPath = strValue;
        if(strName == "word_probability")
            m_strWordProbabilityPath = strValue;
        if(strName == "attribute_word_list")
            m_strAttributeWordListPath = strValue;
        if(strName == "attribute_precision")
            m_strAttributePrecisionPath = strValue;
        if(strName == "attribute_word_number")
            m_strAttributeWordNumPath = strValue;
        if(strName == "loglevel")
            m_logLevel = atoi(strValue.c_str());

		if(strName == "data_source")
			m_strSourcePath = strValue;
		if(strName == "test_data")
			m_strTestPath = strValue;
		if(strName == "training_data")	
			m_strTrainPath = strValue;
		if(strName == "dict_path")	
			m_strDictPath = strValue;
		if(strName == "whitelist_path")	
			m_strWhiteList = strValue;
		if(strName == "confilctlist_path")
			m_strCollfictList = strValue;
		if(strName == "des_good_data")	
			m_strGoodDataPath = strValue;
		if(strName == "des_bad_data")	
			m_strBadDataPath = strValue;
		if(strName == "data_source_bak")	
			m_strBakPath = strValue;
		if(strName == "attribute_list")
		{
			CCommon::split(strValue.c_str(), m_attributeTitle, ',');	
		}
        if(strName == "attribute_proportion")
        {
            list<string> listAttributeProportion;
            CCommon::split(strValue.c_str(),listAttributeProportion,'$');
            list<string>::iterator itAttributeProportion;
            for(itAttributeProportion = listAttributeProportion.begin();itAttributeProportion != listAttributeProportion.end();++itAttributeProportion)
            {
                cout<<*itAttributeProportion<<endl;
                list<string> listNameFloat;
                CCommon::split((*itAttributeProportion).c_str(),listNameFloat,',');
                list<string>::iterator itStringName = listNameFloat.begin();
                list<string>::iterator itStringProportion = itStringName;
                ++itStringProportion;
                m_mapAttrWordProportion.insert(ProportionPair(*itStringName,strtod(itStringProportion->c_str(),NULL)));
                  
            }
        }
        if(strName == "selected_record_path")
        {
            m_strWordSelectedFilePath = strValue;
        }
		if(strName == "attribute_key")
			m_attributeKey = strValue;
			
        if(strName == "feature_selection_result")
        {
            m_strFeatureScore = strValue;
        }	
		
		if(strName == "fenci")
			m_bIfWordSeg = (strValue=="true"||strValue=="True"||strValue=="TRUE")? true: false;
        if(strName == "if_train")
            m_bIfTrain = (strValue == "true"||strValue == "True"||strValue =="TRUE")?true:false;
	}	
	fclose(filePtr);
    
	logInfo(">>>>>>>>    read ok : %s ", strEtcPath.c_str());
	return true;
}
 
void YS_ConfigData::printInfo() 
{
	 logInfo( "configure data %s is as follows: ",  m_strSourcePath.c_str());
	 logInfo( " ");
	 logInfo( "    data_source:%s ",  m_strSourcePath.c_str());
	 logInfo( "    test_data:%s ", m_strTestPath.c_str());
	 logInfo( "    training_data:%s ", m_strTrainPath.c_str());
	 logInfo( "    dict_path:%s ", m_strDictPath.c_str());
	 logInfo( "    whitelist_path:%s ", m_strWhiteList.c_str());
	 logInfo( "    confilctlist_path:%s ", m_strCollfictList.c_str());
	 logInfo( "    des_good_data:%s ", m_strGoodDataPath.c_str());
	 logInfo( "    des_bad_data:%s ", m_strBadDataPath.c_str());
	 logInfo( "    data_source_bak:%s ", m_strBakPath.c_str());
     logInfo( "    attribute_precision:%s ",m_strAttributePrecisionPath.c_str());
     logInfo( "    attribute_word_list:%s ",m_strAttributeWordListPath.c_str());
     logInfo( "    class_probability:%s ",m_strClassProbabilityPath.c_str());
     logInfo( "    word_probability:%s ",m_strWordProbabilityPath.c_str());
     logInfo( "    attribute_word_number:%s ",m_strAttributeWordNumPath.c_str());
     logInfo( "    m_logLevel:%d ",m_logLevel);
	if(m_bIfWordSeg)
		 logInfo( "    training data needs wordsegment ");
	else
		 logInfo( "    training data no needs wordsegment ");
	
	 logInfo( "    attribute Title List: ");
	int iPos=0;
	for(list<string>::iterator it=m_attributeTitle.begin(); it!=m_attributeTitle.end(); ++it)
	{
		if(*it==m_attributeKey)
			 logInfo( "        Attribute Key is:[%d]%s ", iPos,m_attributeKey.c_str());
		else
			 logInfo( "                         [%d]:%s ", iPos,it->c_str());
		++iPos;
	}
} 

/*Ys_BadCharacter info */
Ys_BadCharacter* Ys_BadCharacter::m_instance = NULL;
string Ys_BadCharacter::m_strBadChar(",.~!@#$%^&*)(_+-=/*;':\"?><,./\\！～）（——『』【】，。？《》“”：；、…");
string Ys_BadCharacter::m_strBadChineseChar(",$.$~$!$@$#$%$^$&$*$)$($_$+$-$=$/$*$;$'$:$\"$?$>$<$/$\\$！$～$）$（$——$『$』$【$】$，$。$？$《$》$“$”$：$；$、$…$--$?$“$”$》$－－$able$about$above$according$accordingly$across$actually$after$afterwards$again$against$all$allow$allows$almost$alone$along$already$also$although$always$am$among$amongst$an$and$another$any$anybody$anyhow$anyone$anything$anyway$anyways$anywhere$apart$appear$appreciate$appropriate$are$aren't$around$as$a's$aside$ask$asking$associated$at$available$away$awfully$be$became$because$become$becomes$becoming$been$before$beforehand$behind$being$believe$below$beside$besides$best$better$between$beyond$both$brief$but$by$came$can$cannot$cant$can't$cause$causes$certain$certainly$changes$clearly$c'mon$co$com$come$comes$concerning$consequently$consider$considering$contain$containing$contains$corresponding$could$couldn't$course$c's$currently$definitely$described$despite$did$didn't$different$do$does$doesn't$doing$done$don't$down$downwards$during$each$edu$eg$eight$either$else$elsewhere$enough$entirely$especially$et$etc$even$ever$every$everybody$everyone$everything$everywhere$ex$exactly$example$except$far$few$fifth$first$five$followed$following$follows$for$former$formerly$forth$four$from$further$furthermore$get$gets$getting$given$gives$go$goes$going$gone$got$gotten$greetings$had$hadn't$happens$hardly$has$hasn't$have$haven't$having$he$hello$help$hence$her$here$hereafter$hereby$herein$here's$hereupon$hers$herself$he's$hi$him$himself$his$hither$hopefully$how$howbeit$however$i'd$ie$if$ignored$i'll$i'm$immediate$in$inasmuch$inc$indeed$indicate$indicated$indicates$inner$insofar$instead$into$inward$is$isn't$it$it'd$it'll$its$it's$itself$i've$just$keep$keeps$kept$know$known$knows$last$lately$later$latter$latterly$least$less$lest$let$let's$like$liked$likely$little$look$looking$looks$ltd$mainly$many$may$maybe$me$mean$meanwhile$merely$might$more$moreover$most$mostly$much$must$my$myself$name$namely$nd$near$nearly$necessary$need$needs$neither$never$nevertheless$new$next$nine$no$nobody$non$none$noone$nor$normally$not$nothing$novel$now$nowhere$obviously$of$off$often$oh$ok$okay$old$on$once$one$ones$only$onto$or$other$others$otherwise$ought$our$ours$ourselves$out$outside$over$overall$own$particular$particularly$per$perhaps$placed$please$plus$possible$presumably$probably$provides$que$quite$qv$rather$rd$re$really$reasonably$regarding$regardless$regards$relatively$respectively$right$said$same$saw$say$saying$says$second$secondly$see$seeing$seem$seemed$seeming$seems$seen$self$selves$sensible$sent$serious$seriously$seven$several$shall$she$should$shouldn't$since$six$so$some$somebody$somehow$someone$something$sometime$sometimes$somewhat$somewhere$soon$sorry$specified$specify$specifying$still$sub$such$sup$sure$take$taken$tell$tends$th$than$thank$thanks$thanx$that$thats$that's$the$their$theirs$them$themselves$then$thence$there$thereafter$thereby$therefore$therein$theres$there's$thereupon$these$they$they'd$they'll$they're$they've$think$third$this$thorough$thoroughly$those$though$three$through$throughout$thru$thus$to$together$too$took$toward$towards$tried$tries$truly$try$trying$t's$twice$two$un$under$unfortunately$unless$unlikely$until$unto$up$upon$us$use$used$useful$uses$using$usually$value$various$very$via$viz$vs$want$wants$was$wasn't$way$we$we'd$welcome$well$we'll$went$were$we're$weren't$we've$what$whatever$what's$when$whence$whenever$where$whereafter$whereas$whereby$wherein$where's$whereupon$wherever$whether$which$while$whither$who$whoever$whole$whom$who's$whose$why$will$willing$wish$with$within$without$wonder$won't$would$wouldn't$yes$yet$you$you'd$you'll$your$you're$yours$yourself$yourselves$you've$zero$zt$ZT$zz$ZZ$一$一下$一些$一切$一则$一天$一定$一方面$一旦$一时$一来$一样$一次$一片$一直$一致$一般$一起$一边$一面$万一$上下$上升$上去$上来$上述$上面$下列$下去$下来$下面$不一$不久$不仅$不会$不但$不光$不单$不变$不只$不可$不同$不够$不如$不得$不怕$不惟$不成$不拘$不敢$不断$不是$不比$不然$不特$不独$不管$不能$不要$不论$不足$不过$不问$与$与其$与否$与此同时$专门$且$两者$严格$严重$个$个人$个别$中小$中间$丰富$临$为$为主$为了$为什么$为什麽$为何$为着$主张$主要$举行$乃$乃至$么$之$之一$之前$之后$之後$之所以$之类$乌乎$乎$乘$也$也好$也是$也罢$了$了解$争取$于$于是$于是乎$云云$互相$产生$人们$人家$什么$什么样$什麽$今后$今天$今年$今後$仍然$从$从事$从而$他$他人$他们$他的$代替$以$以上$以下$以为$以便$以免$以前$以及$以后$以外$以後$以来$以至$以至于$以致$们$任$任何$任凭$任务$企图$伟大$似乎$似的$但$但是$何$何况$何处$何时$作为$你$你们$你的$使得$使用$例如$依$依照$依靠$促进$保持$俺$俺们$倘$倘使$倘或$倘然$倘若$假使$假如$假若$做到$像$允许$充分$先后$先後$先生$全部$全面$兮$共同$关于$其$其一$其中$其二$其他$其余$其它$其实$其次$具体$具体地说$具体说来$具有$再者$再说$冒$冲$决定$况且$准备$几$几乎$几时$凭$凭借$出去$出来$出现$分别$则$别$别的$别说$到$前后$前者$前进$前面$加之$加以$加入$加强$十分$即$即令$即使$即便$即或$即若$却不$原来$又$及$及其$及时$及至$双方$反之$反应$反映$反过来$反过来说$取得$受到$变成$另$另一方面$另外$只是$只有$只要$只限$叫$叫做$召开$叮咚$可$可以$可是$可能$可见$各$各个$各人$各位$各地$各种$各级$各自$合理$同$同一$同时$同样$后来$后面$向$向着$吓$吗$否则$吧$吧哒$吱$呀$呃$呕$呗$呜$呜呼$呢$周围$呵$呸$呼哧$咋$和$咚$咦$咱$咱们$咳$哇$哈$哈哈$哉$哎$哎呀$哎哟$哗$哟$哦$哩$哪$哪个$哪些$哪儿$哪天$哪年$哪怕$哪样$哪边$哪里$哼$哼唷$唉$啊$啐$啥$啦$啪达$喂$喏$喔唷$嗡嗡$嗬$嗯$嗳$嘎$嘎登$嘘$嘛$嘻$嘿$因$因为$因此$因而$固然$在$在下$地$坚决$坚持$基本$处理$复杂$多$多少$多数$多次$大$大力$大多数$大大$大家$大批$大约$大量$失去$她$她们$她的$好的$好象$如$如上所述$如下$如何$如其$如果$如此$如若$存在$宁$宁可$宁愿$宁肯$它$它们$它们的$它的$安全$完全$完成$实现$实际$宣布$容易$密切$对$对于$对应$将$少数$尔后$尚且$尤其$就$就是$就是说$尽$尽管$属于$岂但$左右$巨大$巩固$己$已经$帮助$常常$并$并不$并不是$并且$并没有$广大$广泛$应当$应用$应该$开外$开始$开展$引起$强烈$强调$归$当$当前$当时$当然$当着$形成$彻底$彼$彼此$往$往往$待$後来$後面$得$得出$得到$心里$必然$必要$必须$怎$怎么$怎么办$怎么样$怎样$怎麽$总之$总是$总的来看$总的来说$总的说来$总结$总而言之$恰恰相反$您$意思$愿意$慢说$成为$我$我们$我的$或$或是$或者$战斗$所$所以$所有$所谓$打$扩大$把$抑或$拿$按$按照$换句话说$换言之$据$掌握$接着$接著$故$故此$整个$方便$方面$旁人$无宁$无法$无论$既$既是$既然$时候$明显$明确$是$是否$是的$显然$显著$普通$普遍$更$更加$曾经$替$最后$最大$最好$最後$最近$最高$有$有些$有关$有利$有力$有所$有效$有时$有点$有的$有着$有著$望$朝$朝着$本$本着$来$来着$极了$构成$果然$果真$某$某个$某些$根据$根本$欢迎$正在$正如$正常$此$此外$此时$此间$毋宁$每$每个$每天$每年$每当$比$比如$比方$比较$毫不$没有$沿$沿着$注意$深入$清楚$满足$漫说$焉$然则$然后$然後$然而$照$照着$特别是$特殊$特点$现代$现在$甚么$甚而$甚至$用$由$由于$由此可见$的$的话$目前$直到$直接$相似$相信$相反$相同$相对$相对而言$相应$相当$相等$省得$看出$看到$看来$看看$看见$真是$真正$着$着呢$矣$知道$确定$离$积极$移动$突出$突然$立即$第$等$等等$管$紧接着$纵$纵令$纵使$纵然$练习$组成$经$经常$经过$结合$结果$给$绝对$继续$继而$维持$综上所述$罢了$考虑$者$而$而且$而况$而外$而已$而是$而言$联系$能$能否$能够$腾$自$自个儿$自从$自各儿$自家$自己$自身$至$至于$良好$若$若是$若非$范围$莫若$获得$虽$虽则$虽然$虽说$行为$行动$表明$表示$被$要$要不$要不是$要不然$要么$要是$要求$规定$觉得$认为$认真$认识$让$许多$论$设使$设若$该$说明$诸位$谁$谁知$赶$起$起来$起见$趁$趁着$越是$跟$转动$转变$转贴$较$较之$边$达到$迅速$过$过去$过来$运用$还是$还有$这$这个$这么$这么些$这么样$这么点儿$这些$这会儿$这儿$这就是说$这时$这样$这点$这种$这边$这里$这麽$进入$进步$进而$进行$连$连同$适应$适当$适用$逐步$逐渐$通常$更多$通过$造成$遇到$遭到$避免$那$那个$那么$那么些$那么样$那些$那会儿$那儿$那时$那样$那边$那里$那麽$部分$鄙人$采取$里面$重大$重新$重要$鉴于$问题$防止$阿$附近$限制$除$除了$除此之外$除非$随$随着$随著$集中$需要$非但$非常$非徒$靠$顺$顺着$首先$高兴$是不是$说说");
Ys_BadCharacter* Ys_BadCharacter::GetInstance()
{
	if( NULL == m_instance)
		m_instance = new Ys_BadCharacter();
	return m_instance;
}

bool Ys_BadCharacter::StartUp()
{
	typedef list<string> StrList;
	typedef StrList::iterator StrListIter;
	m_hashBadCharSet.clear();
	StrList attrList;
	CCommon::split(m_strBadChineseChar.c_str(), attrList, SPLIT_CHAR);
	for(StrListIter it=attrList.begin(); it!=attrList.end(); ++it)
	{
		if(!m_hashBadCharSet.insert(it->c_str()).second)
			ERR_LOG("bad char[%s]insert fault", it->c_str());
	}
	return true;
}

bool Ys_BadCharacter::findBadChar(const char* ch)
{
	return findBadChar(string(ch));
}

bool Ys_BadCharacter::findBadChar(const string& item)
{
	if(item.size()< MIN_CHAR_LEN) return true;  //字符串长度过滤 过滤 单个英文字符 
	
	if(m_hashBadCharSet.find(item) != m_hashBadCharSet.end())
	{
		//INF_LOG("%s is a bad chinese char", item.c_str());
		return true;
	}
	else
		return false;
}

bool Ys_BadCharacter::earseInvalidChar(string& str)
{
	string::size_type iPos = str.find("(");
	if(iPos != string::npos)
		str = str.substr(0, iPos);

	iPos = str.find("（");
	if(iPos != string::npos)
		str = str.substr(0, iPos);
		
	return true;
}

YS_WordSegmentAgent* YS_WordSegmentAgent::m_instance = NULL;
YS_WordSegmentAgent* YS_WordSegmentAgent::GetInstance()
{
	if(m_instance == NULL)
		m_instance = new YS_WordSegmentAgent();
	return m_instance;
}
YS_WordSegmentAgent::YS_WordSegmentAgent()
{
	m_wordSegmentLR = new YS_WordSegmentLR();
};

bool YS_WordSegmentAgent::StartUp()
{
	list<string> flieList;
	if( !CCommon::GetDirFilePath(YS_ConfigData::GetInstance()->m_strDictPath, flieList))
	{
		ERR_LOG("GetDirFilePath is error ");
		return false;
	}

	if(-1 == m_wordSegmentLR->Init(flieList)){
		ERR_LOG("Load lr dict %s failed. ", YS_ConfigData::GetInstance()->m_strDictPath.c_str());
		return false;;
	}
	return true;
}

YS_WordSegmentAgent::~YS_WordSegmentAgent()
{
	delete m_wordSegmentLR;
}

void YS_WordSegmentAgent::ExtractWordSegment(const char* szKeyword, list<YSS_Word>& lstWord)
{
	m_wordSegmentLR->ExtractWordSegment(szKeyword, lstWord);
}

void YS_WordSegmentAgent::DestroyWordSegment(list<YSS_Word>& lstWord)
{
	m_wordSegmentLR->DestroyWordSegment(lstWord);
}

/*signal info*/
SigFunc* mysignal(int signo, SigFunc* func)
{
	struct sigaction act, oact;

	act.sa_handler = func;
	sigfillset(&act.sa_mask);
	act.sa_flags = 0;
	if ((signo == SIGALRM) || (signo == SIGPIPE))
	{
#ifdef SA_INTERRUPT
		act.sa_flags |= SA_INTERRUPT;
#endif
	}
	else
	{
#ifdef SA_RESTART
		act.sa_flags |= SA_RESTART;
#endif
	}
	if (sigaction(signo, &act, &oact) < 0) return (SIG_ERR);
	return (oact.sa_handler);
}

int TOOLS::IsTextUTF8(unsigned char chr)
{
	if(chr>=0x80)
	{
		if(chr>=0xFC&&chr<=0xFD)
			return 6;
		else if(chr>=0xF8)
			return 5;
		else if(chr>=0xF0)
			return 4;
		else if(chr>=0xFC)
			return 3;
		else if(chr>=0xC0)
			return 2;
	}
	else
		return 1;
}

bool TOOLS::IsGbkCode(char const* src){
    if(src[0]==0x00 || src[1]==0x00) return false;
    if ( ((unsigned char)src[0] >=0x81) &&
         ((unsigned char)src[0] <=0xFE) &&
         ((unsigned char)src[1] >=0x40) &&
         ((unsigned char)src[1] <=0xFE) ) return true;
    return false;
}

bool TOOLS::reverseWord(const string& src, string& des)
{
    const char* strValue = src.c_str();
    int iLen = src.length();
    char szBuf[BUF_LEN];
    szBuf[0] = 0x00;
    for(int i=0; i<iLen; )
    {
        if(IsGbkCode(strValue+i))
        {
            memcpy(szBuf+i, strValue+iLen-i-2, 2);
            i += 2;
        }
        else
        {
            int l = IsTextUTF8(strValue[i]);
            memcpy(szBuf+i, strValue+iLen-i-l, l);
            i += l;
        }
    }
    szBuf[iLen] = 0x00;
    des = szBuf;
    return true;
}

bool TOOLS::partialWord(const string& src, list<string>& des)
{
    des.clear();
    const char* strValue = src.c_str();
    int iLen = src.length();
    char szBuf[BUF_LEN];
    szBuf[0] = 0x00;
    for(int i=0; i<iLen; )
    {
        if(IsGbkCode(strValue+i))
        {
            memcpy(szBuf, strValue+iLen-i-2, 2);
            i += 2;
            szBuf[2] = 0x00;
            des.push_back(string(szBuf));
        }
        else
        {
            int l = IsTextUTF8(strValue[i]);
            memcpy(szBuf, strValue+iLen-i-l, l);
            i += l;
            szBuf[l] = 0x00;
            des.push_back(string(szBuf));
        }
    }
    return true;
}
