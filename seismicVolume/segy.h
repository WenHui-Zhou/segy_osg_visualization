#include <iostream>
#include <cstring>
#include <fstream>
#include <iostream>
#include <tchar.h>
#include <vector>
#include <osg/Geode>
#include <osg/Geometry>
#include <winsock2.h>
#pragma  comment(lib,"ws2_32.lib")
#include <chrono>

using namespace std;

/*
二进制文件头 如果要使用的话需要变换
*/
struct binaryFileHeader{
    int JOB_ID;                
    int LINE_NUM;
    int REEL_NUM;
    short int NUM_OF_TRACE;
    short int NUM_OF_AUX;
    short int INTERVAL_MS;
    short int INTERVAL_MS_ORI;
    unsigned short int NUM_OF_SAMPLES;				// number of samples 一个trace中的样本点
    unsigned short int NUM_OF_SAMPLES_ORI;
    short int SAMPLE_FORMAT;
    short int ENSEMBLE;
    short int TRACE_SORT;
    short int VERT_SUM;
    short int SWEEP_FREQ_START;
    short int SWEEP_FREQ_END;
    short int SWEEP_LENGTH;
    short int SWEEP_TYPE;
    short int SWEEP_NUM_CHANNEL;
    short int SWEEP_TAPER_LEN_START;
    short int SWEEP_TAPER_LEN_END;
    short int TAPER_TYPE;
    short int CORRELATED;
    short int BINARY_GAIN;
    short int AMP_RECOR;
    short int MEASURE_SYSTEM;
    short int IMPULSE_POLAR;
    short int POLAR_CODE;
    char UNNASSIGNED1 [240];
    short int SEGY_REV_NUM;
    short int FIXED_LEN;
    short int NUM_EXT_HEAD;
    char UNNASSIGNED2 [94];
};

/*
道头数据
*/
struct traceHeader {
    int TRACE_SEQ_GLOBAL;
    int TRACE_SEQ_LOCAL;
    int ORI_RECORD_NUM;
    int TRACE_NUM_FIELD;
    int SOURCE_POINT;
    int ENSEMBLE_NUM;
    int ENS_TRACE_NUM;
    short int TRACE_CODE;
    short int NUM_VERT_SUM;
    short int NUM_HORZ_SUM;
    short int DATA_USE;
    int DIST_CENT_RECV;
    int RECV_GRP_ELEV;
    int SURF_ELEV_SRC;
    int SOURCE_DEPTH;
    int DATUM_ELEV_RECV;
    int DATUM_ELAV_SRC;
    int WATER_DEPTH_SRC;
    int WATER_DEPTH_GRP;
    short int SCALE_DEPTH;
    short int SCALE_COOR;
    int SRC_COOR_X;
    int SRC_COOR_Y;
    int GRP_COOR_X;
    int GRP_COOR_Y;
    short int COOR_UNIT;
    short int WEATHER_VEL;
    short int SWEATHER_VEL;
    short int UPHOLE_T_SRC;
    short int UPHOLE_T_GRP;
    short int SRC_STA_CORRC;
    short int GRP_STA_CORRC;
    short int TOTAL_STA;
    short int LAG_TIME_A;
    short int LAG_TIME_B;
    short int DELAY_T;
    short int MUTE_T_STRT;
    short int MUTE_T_END;
    unsigned short int NUM_OF_SAMPL;				// 每一个道中的样本数
    unsigned short int SAMPLE_INTRVL;				// 样本的间隔
    short int GAIN_TYPE;
    short int GAIN_CONST;
    short int GAIN_INIT;
    short int CORRLTD;
    short int SWEEP_FREQ_START;
    short int SWEEP_FREQ_END;
    short int SWEEP_LENGTH;
    short int SWEEP_TYPE;
    short int SWEEP_TAPER_LEN_START;
    short int SWEEP_TAPER_LEN_END;
    short int TAPER_TYPE;
    short int ALIAS_FREQ;
    short int ALIAS_SLOPE;
    short int NOTCH_FREQ;
    short int NOTCH_SLOPE;
    short int LOWCUT_FREQ;
    short int HIGHCUT_FREQ;
    short int LOWCUT_SLOPE;
    short int HIGHCUT_SLOPE;
    short int YEAR;
    short int DAY;
    short int HOUR;
    short int MINUTE;
    short int SECOND;
    short int TIME_CODE;
    short int WEIGHT_FACT;
    short int GEOPHNE_ROLL;
    short int GEOPHNE_TRACE;
    short int GEOPHNE_LAST;
    short int GAP_SIZE;
    short int OVER_TRAVEL;
    int ENS_COOR_X;
    int ENS_COOR_Y;
    int INLINE;										//inline值
    int CROSS;										//crossline值
    int SHOOTPOINT;
    short int SHOOTPOINT_SCALE;
    short int TRACE_UNIT;
    char TRANSD_CONST [6];
    short int TRANSD_UNIT;
    short int TRACE_IDENT;
    short int SCALE_TIME;
    short int SRC_ORIENT;
    char SRC_DIRECTION [6];
    char SRC_MEASUREMT [6];
    short int SRC_UNIT;
    char UNNASSIGNED1 [6];
};
/*
用查表的方式将ebcdic编码转换为ascii码格式
*/
static const unsigned char e2a[256] = {
          0,  1,  2,  3,156,  9,134,127,151,141,142, 11, 12, 13, 14, 15,
         16, 17, 18, 19,157,133,  8,135, 24, 25,146,143, 28, 29, 30, 31,
        128,129,130,131,132, 10, 23, 27,136,137,138,139,140,  5,  6,  7,
        144,145, 22,147,148,149,150,  4,152,153,154,155, 20, 21,158, 26,
         32,160,161,162,163,164,165,166,167,168, 91, 46, 60, 40, 43, 33,
         38,169,170,171,172,173,174,175,176,177, 93, 36, 42, 41, 59, 94,
         45, 47,178,179,180,181,182,183,184,185,124, 44, 37, 95, 62, 63,
        186,187,188,189,190,191,192,193,194, 96, 58, 35, 64, 39, 61, 34,
        195, 97, 98, 99,100,101,102,103,104,105,196,197,198,199,200,201,
        202,106,107,108,109,110,111,112,113,114,203,204,205,206,207,208,
        209,126,115,116,117,118,119,120,121,122,210,211,212,213,214,215,
        216,217,218,219,220,221,222,223,224,225,226,227,228,229,230,231,
        123, 65, 66, 67, 68, 69, 70, 71, 72, 73,232,233,234,235,236,237,
        125, 74, 75, 76, 77, 78, 79, 80, 81, 82,238,239,240,241,242,243,
         92,159, 83, 84, 85, 86, 87, 88, 89, 90,244,245,246,247,248,249,
         48, 49, 50, 51, 52, 53, 54, 55, 56, 57,250,251,252,253,254,255
};

/*
segy文件读取，可视化类
*/
class segy
{
public:
	segy();
    ~segy();
    void ReadAllTrace();												//读取所有的地震数据
	void ReadTraceHeader(std::string fl);                               //读取前3200字段和400字段，读出inline,xline,以及sample of trace
	void ReadOneTrace();								                //读取一个切面地震道数据
    void PrintTraceHeader();                                            //地震道道头数据240字节
    void PrintTextHeader();                                             //输出文件起始位置的3200字节
    void PrintBinaryHeader();                                           //输出二进制文件头400字节
	void getBuf(const char* const src,char* const &buf,
		const int &offset,const int &len);
	void switchByte(char* const c,const int &len);
    void drawAllTrace();                                                //绘制面
	void colorMap();                                                    //强度--颜色映射
	void readFaceData();                                                //读出顶点数据
	void setUnitGeom(osg::ref_ptr<osg::Geometry> geom);                 //设置图元单元
    unsigned char TFileHead_ [3200];
    unsigned char ExTFileHead_ [3200];
    binaryFileHeader BFileHead_;
    traceHeader traceHeader_;
    float **Trace_;														//二维的地震道矩阵
	float ***VolumeTrace;												//三维地震数据，生成的数据 VolumeTrace[inline-1][reaceNum-1][sampleNum-1]
	int fileLength;														//文件长度
	int traceNum;														// 地震道的道数   
	int sampleNum;														//采样点数
	int InLine;															//纵向上的数量
	int CrossLine;														//地震道数，横向上的数量，等于traceNum
	float maxValue;														//最大的地震强度值，用于标准化地震强度（颜色映射）
	std::vector<float> intensity;										//用于存放地震强度数据（存放需要绘制的点数据）
	FILE *ff;															//SEGY文件指针
	void outputFile();													//导出文件
    std::string filename_;
	osg::ref_ptr<osg::Vec3Array> data;
	osg::ref_ptr<osg::Vec4Array> color;
private:
    int toLitteEnd(int);
    unsigned int toLitteEnd(unsigned int);
    unsigned short int toLitteEnd(unsigned short int);
    short int toLitteEnd( short int);
    float toIeee(uint32_t ibm);
};

segy::segy(){
	data = new osg::Vec3Array;
	color = new osg::Vec4Array;
	maxValue = 0.0;
}

segy::~segy(){
}

int segy::toLitteEnd(int a){
    unsigned short int tmp1=(a>>16);
    unsigned short int tmp2=(a* 0x0000FFFF);
    tmp2 = toLitteEnd(tmp2);
    tmp1 = toLitteEnd(tmp1);
    
    int b = (int)tmp2;
    b = b << 16;
    b = b | (int)tmp1;

    return b;
}

unsigned int segy::toLitteEnd(unsigned int a){
    unsigned short int tmp1=(a>>16);
    unsigned short int tmp2=(a* 0x0000FFFF);
    tmp2 = toLitteEnd(tmp2);
    tmp1 = toLitteEnd(tmp1);
    
    unsigned int b = (unsigned int)tmp2;
    b = b << 16;
    b = b | (unsigned int)tmp1;

    return b;
}

unsigned short int segy::toLitteEnd(unsigned short int a){
    unsigned short int tmp = a>>8;
    return (a<<8) | (tmp);
}

short int segy::toLitteEnd( short int a){
    short int tmp = a>>8;
    return (a<<8) | (tmp);
}
/*
ibm编码转变为ieee编码
*/
float segy::toIeee(uint32_t ibm)   
{
    ibm = ntohl(ibm);
    uint32_t fr; /* fraction */
    int32_t exp; /* exponent */
    int32_t sgn; /* sign */
    fr = ibm;
    sgn = (int32_t)(fr >> 31); /* save sign */
    fr <<= 1; /* shift sign out */
    exp = (int32_t)(fr >> 25); /* save exponent */
    fr <<= 7; /* shift exponent out */
    if (fr == 0)
    {
        exp = 0;
        goto done;
    }
    exp = (exp << 2) - 130;
    while (fr < 0x80000000)
    {
        --exp;
        fr <<= 1;
    }
    if (exp <= 0)
    {
        if (exp < -24)
            fr = 0;
        else 
            fr >>= -exp;
        exp = 0;
    }
    else if (exp >= 255)
    { 
        fr = 0;
        exp = 255;
    }
    else
    { 
        fr <<= 1;
    }
    done:
        uint32_t ieee = ((uint32_t)(fr >> 9)) | ((uint32_t)(exp << 23)) | ((uint32_t)(sgn << 31));
        float result;
        memcpy(&result, &ieee, sizeof(float));
        return result;
}

   
/*
文件长度减去3600的道头，剩下的长度就是地震道道头与数据的长度。
toLitteEnd(BFileHead_.NUM_OF_SAMPLES)是指一个地震道中的样本点数，每一个4字节
240字节指的是道头的长度。
*/
void segy::ReadTraceHeader(std::string fl)
{
	char temp [4000];  
    filename_ = fl;
	std::fstream in_;
    in_.open(filename_.c_str(),std::ifstream::in);					//读入文件
    if(in_.fail()) 
    {
        printf("Error opening file %s \n",filename_.c_str());
        exit(0);
    }
	in_.seekg (0, in_.end);											//将指针移动到文件末尾
	fileLength = in_.tellg();										//得到文件的长度
	in_.seekg(0,in_.beg);											//重新将指针移动到文件头
    memset( TFileHead_, '\0', sizeof(char) * 3200 );				//内存初始化，全初始化为0
    in_.read((char *)TFileHead_, sizeof(TFileHead_));				//将前3200字节写入TFileHead中
    in_.read(temp, sizeof(BFileHead_));								//向temp中写入二进制流即400字节的内容
    std::memcpy(&BFileHead_,temp,sizeof(BFileHead_));				//将二进制流的内容拷贝到BFileHead_中去
	sampleNum = toLitteEnd(BFileHead_.NUM_OF_SAMPLES);				//采样点数
	CrossLine = traceNum = toLitteEnd(BFileHead_.NUM_OF_TRACE);		// CrossLine的值
	if (CrossLine==1||CrossLine==0)									//单片,当头文件中BFileHead_.NUM_OF_TRACE 为零时，代表他是二维的数据corssLine重新计算
	{
		InLine = 0;
		traceNum = CrossLine = (fileLength-3600)/(240+4*sampleNum);	//地震道道数
	}
	else															//三维数据 包含多个面
	{	
		InLine = (fileLength-3600)/(240+4*sampleNum)/CrossLine;		//地震道道数
	}
	in_.read(temp, sizeof(traceHeader_));							//读取地震道数据头数据（第一个地震道的道头信息）
	std::memcpy(&traceHeader_,temp,sizeof(traceHeader_));			//将地震道头数据240字节写到traceHeader中去
	printf("inline: %d  CorssLine: %d\n",InLine,CrossLine);
	printf("number of sample in filehead: %d ",sampleNum);
	in_.close();
}
/*
读取得到单个面的地震数据
地震数据记录反射波的振幅大小，包含正负以及0
地震数据PC-IEEE类型：数值在-10571值10571之间（某底层数据，可是大概估计数字读取是否正确）
地震数据IBM-IEEE类型：数值在-127至127之间
*/
void segy::ReadOneTrace()
{
	Trace_ = (float**)malloc(sizeof(float)*CrossLine);									//申请了traceNum个地震道空间
	for (int i = 0; i < CrossLine; i++)
	{
		Trace_[i] = (float*)malloc(sizeof(float)*sampleNum);							//为每一个地震道申请 BFileHead_.NUM_OF_SAMPLES 个采样点的空间
	}
	int iTrace = 0;
	while(iTrace<CrossLine)
	{
																						//地震道数据的读取：
		fseek(ff,240,SEEK_CUR);														    //从当前位置往后读写
		if (toLitteEnd(BFileHead_.SAMPLE_FORMAT) == 5)									//PC IEEE 类型
		{
			char *buf(nullptr);
			buf = new char[sampleNum*sizeof(float)];									// 申请保存一道的数据空间
			fread(buf,sampleNum*sizeof(float),1,ff);									// 将数据读到buf中
			char _char2int[4];
			for (int index = 0;index<sampleNum;index++)
			{
				getBuf(buf, _char2int, index*4, 4);
				memcpy(&Trace_[iTrace][index], _char2int, 4);
			}
			delete[]buf;
		}
		else if(toLitteEnd(BFileHead_.SAMPLE_FORMAT) == 1)								// IBM IEEE类型 需要转换为PC IEEE
		{
			uint8_t* buffer = new uint8_t[sampleNum*sizeof(float)];
			fread(reinterpret_cast<char*>(buffer),sampleNum*sizeof(float),1,ff);		// 将数据读到buf中
			char _char2int[4];
			for (size_t n = 0; n < sampleNum; n++)
			{
				uint32_t ibmSample = *(reinterpret_cast<uint32_t*>(buffer + n*4));
				Trace_[iTrace][n] = toIeee(ibmSample);
			}
		}
		iTrace++;																		// 控制道数
	}
}
/*
读取所有的地震数据
VolumeTrace：三维数组存储所有的地震数据
InLine==0表明为二维平面数据否则为三维数据
*/
void segy::ReadAllTrace()
{
	ff = fopen(filename_.c_str(),"rb");
	if (ff==NULL)
	{
		printf("Error opening file %s \n",filename_.c_str());
		exit(0);
	}
	fseek(ff,3600,SEEK_SET);												//从文件起始位置开始
	if (InLine!=0)															//体数据
	{
		VolumeTrace = (float***)malloc(sizeof(float)*CrossLine*InLine);
		for (int i = 0; i < InLine; i++)
		{
			ReadOneTrace();
			VolumeTrace[i] = Trace_;
		}
	}
	else																	//面数据
	{
		ReadOneTrace();														//读到的数据在trace_中
	}
	fclose(ff);
}

void segy::PrintTraceHeader(){
    printf("TRACE_SEQ_GLOBAL    : %i \n",toLitteEnd( traceHeader_.TRACE_SEQ_GLOBAL) );
    printf("TRACE_SEQ_LOCAL     : %i \n", toLitteEnd( traceHeader_.TRACE_SEQ_LOCAL) );
    printf("ORI_RECORD_NUM      : %i \n",  toLitteEnd( traceHeader_.ORI_RECORD_NUM) );
    printf("TRACE_NUM_FIELD     : %i \n", toLitteEnd( traceHeader_.TRACE_NUM_FIELD) );
    printf("SOURCE_POINT        : %i \n",    toLitteEnd( traceHeader_.SOURCE_POINT) );
    printf("ENSEMBLE_NUM        : %i \n",    toLitteEnd( traceHeader_.ENSEMBLE_NUM) );
    printf("ENS_TRACE_NUM       : %i \n",   toLitteEnd( traceHeader_.ENS_TRACE_NUM) );
    printf("TRACE_CODE          : %hi \n",     toLitteEnd( traceHeader_.TRACE_CODE) );
    printf("NUM_VERT_SUM        : %hi \n",   toLitteEnd( traceHeader_.NUM_VERT_SUM) );
    printf("NUM_HORZ_SUM        : %hi \n",   toLitteEnd( traceHeader_.NUM_HORZ_SUM) );
    printf("DATA_USE            : %hi \n",       toLitteEnd( traceHeader_.DATA_USE) );
    printf("DIST_CENT_RECV      : %i \n",  toLitteEnd( traceHeader_.DIST_CENT_RECV) );
    printf("RECV_GRP_ELEV       : %i \n",   toLitteEnd( traceHeader_.RECV_GRP_ELEV) );
    printf("SURF_ELEV_SRC       : %i \n",   toLitteEnd( traceHeader_.SURF_ELEV_SRC) );
    printf("SOURCE_DEPTH        : %i \n",    toLitteEnd( traceHeader_.SOURCE_DEPTH) );
    printf("DATUM_ELEV_RECV     : %i \n", toLitteEnd( traceHeader_.DATUM_ELEV_RECV) );
    printf("DATUM_ELAV_SRC      : %i \n",  toLitteEnd( traceHeader_.DATUM_ELAV_SRC) );
    printf("WATER_DEPTH_SRC     : %i \n", toLitteEnd( traceHeader_.WATER_DEPTH_SRC) );
    printf("WATER_DEPTH_GRP     : %i \n", toLitteEnd( traceHeader_.WATER_DEPTH_GRP) );
    printf("SCALE_DEPTH         : %hi \n",    toLitteEnd( traceHeader_.SCALE_DEPTH) );
    printf("SCALE_COOR          : %hi \n",     toLitteEnd( traceHeader_.SCALE_COOR) );
    printf("SRC_COOR_X          : %i \n",      toLitteEnd( traceHeader_.SRC_COOR_X) );
    printf("SRC_COOR_Y          : %i \n",      toLitteEnd( traceHeader_.SRC_COOR_Y) );
    printf("GRP_COOR_X          : %i \n",      toLitteEnd( traceHeader_.GRP_COOR_X) );
    printf("GRP_COOR_Y          : %i \n",      toLitteEnd( traceHeader_.GRP_COOR_Y) );
    printf("COOR_UNIT           : %hi \n",      toLitteEnd( traceHeader_.COOR_UNIT) );
    printf("WEATHER_VEL         : %hi \n",    toLitteEnd( traceHeader_.WEATHER_VEL) );
    printf("SWEATHER_VEL        : %hi \n",   toLitteEnd( traceHeader_.SWEATHER_VEL) );
    printf("UPHOLE_T_SRC        : %hi \n",   toLitteEnd( traceHeader_.UPHOLE_T_SRC) );
    printf("UPHOLE_T_GRP        : %hi \n",   toLitteEnd( traceHeader_.UPHOLE_T_GRP) );
    printf("SRC_STA_CORRC       : %hi \n",  toLitteEnd( traceHeader_.SRC_STA_CORRC) );
    printf("GRP_STA_CORRC       : %hi \n",  toLitteEnd( traceHeader_.GRP_STA_CORRC) );
    printf("TOTAL_STA           : %hi \n",      toLitteEnd( traceHeader_.TOTAL_STA) );
    printf("LAG_TIME_A          : %hi \n",     toLitteEnd( traceHeader_.LAG_TIME_A) );
    printf("LAG_TIME_B          : %hi \n",     toLitteEnd( traceHeader_.LAG_TIME_B) );
    printf("DELAY_T             : %hi \n",        toLitteEnd( traceHeader_.DELAY_T) );
    printf("MUTE_T_STRT         : %hi \n",    toLitteEnd( traceHeader_.MUTE_T_STRT) );
    printf("MUTE_T_END          : %hi \n",     toLitteEnd( traceHeader_.MUTE_T_END) );
    printf("NUM_OF_SAMPL        : %hi \n",   toLitteEnd( traceHeader_.NUM_OF_SAMPL) );
    printf("SAMPLE_INTRVL       : %hi \n", toLitteEnd( traceHeader_.SAMPLE_INTRVL) );
    printf("GAIN_TYPE           : %hi \n", toLitteEnd( traceHeader_.GAIN_TYPE) );
    printf("GAIN_CONST          : %hi \n", toLitteEnd( traceHeader_.GAIN_CONST) );
    printf("GAIN_INIT           : %hi \n", toLitteEnd( traceHeader_.GAIN_INIT) );
    printf("CORRLTD             : %hi \n", toLitteEnd( traceHeader_.CORRLTD) );
    printf("SWEEP_FREQ_START    : %hi \n", toLitteEnd( traceHeader_.SWEEP_FREQ_START) );
    printf("SWEEP_FREQ_END      : %hi \n", toLitteEnd( traceHeader_.SWEEP_FREQ_END) );
    printf("SWEEP_LENGTH        : %hi \n", toLitteEnd( traceHeader_.SWEEP_LENGTH) );
    printf("SWEEP_TYPE          : %hi \n", toLitteEnd( traceHeader_.SWEEP_TYPE) );
    printf("SWEEP_TAPER_LEN_START   : %hi \n", toLitteEnd( traceHeader_.SWEEP_TAPER_LEN_START) );
    printf("SWEEP_TAPER_LEN_END : %hi \n", toLitteEnd( traceHeader_.SWEEP_TAPER_LEN_END) );
    printf("TAPER_TYPE          : %hi \n", toLitteEnd( traceHeader_.TAPER_TYPE) );
    printf("ALIAS_FREQ          : %hi \n", toLitteEnd( traceHeader_.ALIAS_FREQ) );
    printf("ALIAS_SLOPE         : %hi \n", toLitteEnd( traceHeader_.ALIAS_SLOPE) );
    printf("NOTCH_FREQ          : %hi \n", toLitteEnd( traceHeader_.NOTCH_FREQ) );
    printf("NOTCH_SLOPE         : %hi \n", toLitteEnd( traceHeader_.NOTCH_SLOPE) );
    printf("LOWCUT_FREQ         : %hi \n", toLitteEnd( traceHeader_.LOWCUT_FREQ) );
    printf("HIGHCUT_FREQ        : %hi \n", toLitteEnd( traceHeader_.HIGHCUT_FREQ) );
    printf("LOWCUT_SLOPE        : %hi \n", toLitteEnd( traceHeader_.LOWCUT_SLOPE) );
    printf("HIGHCUT_SLOPE       : %hi \n", toLitteEnd( traceHeader_.HIGHCUT_SLOPE) );
    printf("YEAR                : %hi \n", toLitteEnd( traceHeader_.YEAR) );
    printf("DAY                 : %hi \n", toLitteEnd( traceHeader_.DAY) );
    printf("HOUR                : %hi \n", toLitteEnd( traceHeader_.HOUR) );
    printf("MINUTE              : %hi \n", toLitteEnd( traceHeader_.MINUTE) );
    printf("SECOND              : %hi \n", toLitteEnd( traceHeader_.SECOND) );
    printf("TIME_CODE           : %hi \n", toLitteEnd( traceHeader_.TIME_CODE) );
    printf("WEIGHT_FACT         : %hi \n", toLitteEnd( traceHeader_.WEIGHT_FACT) );
    printf("GEOPHNE_ROLL        : %hi \n", toLitteEnd( traceHeader_.GEOPHNE_ROLL) );
    printf("GEOPHNE_TRACE       : %hi \n", toLitteEnd( traceHeader_.GEOPHNE_TRACE) );
    printf("GEOPHNE_LAST        : %hi \n", toLitteEnd( traceHeader_.GEOPHNE_LAST) );
    printf("GAP_SIZE            : %hi \n", toLitteEnd( traceHeader_.GAP_SIZE) );
    printf("OVER_TRAVEL         : %hi \n", toLitteEnd( traceHeader_.OVER_TRAVEL) );
    printf("ENS_COOR_X          : %i \n",  toLitteEnd( traceHeader_.ENS_COOR_X) );
    printf("ENS_COOR_Y          : %i \n",  toLitteEnd( traceHeader_.ENS_COOR_Y) );
    printf("INLINE              : %i \n",  toLitteEnd( traceHeader_.INLINE) );
    printf("CROSS               : %i \n",  toLitteEnd( traceHeader_.CROSS) );
    printf("SHOOTPOINT          : %i \n",  toLitteEnd( traceHeader_.SHOOTPOINT) );
    printf("SHOOTPOINT_SCALE    : %hi \n", toLitteEnd( traceHeader_.SHOOTPOINT_SCALE) );
    printf("TRACE_UNIT          : %hi \n", toLitteEnd( traceHeader_.TRACE_UNIT) );
    printf("TRANSD_UNIT         : %hi \n", toLitteEnd( traceHeader_.TRANSD_UNIT) );
    printf("TRACE_IDENT         : %hi \n", toLitteEnd( traceHeader_.TRACE_IDENT) );
    printf("SCALE_TIME          : %hi \n", toLitteEnd( traceHeader_.SCALE_TIME) );
    printf("SRC_ORIENT          : %hi \n", toLitteEnd( traceHeader_.SRC_ORIENT) );
    printf("SRC_UNIT            : %hi \n", toLitteEnd( traceHeader_.SRC_UNIT) );
}

void segy::PrintBinaryHeader(){
    printf("JOB_ID               :%i \n",  toLitteEnd(BFileHead_.JOB_ID     )         );
    printf("LINE_NUM             :%i \n",  toLitteEnd(BFileHead_.LINE_NUM   ) );
    printf("REEL_NUM             :%i \n",  toLitteEnd(BFileHead_.REEL_NUM   ) );
    printf("NUM_OF_TRACE         :%hi \n", toLitteEnd(BFileHead_.NUM_OF_TRACE )   );
    printf("NUM_OF_AUX           :%hi \n", toLitteEnd(BFileHead_.NUM_OF_AUX ) );
    printf("INTERVAL_M           :%hi \n", toLitteEnd(BFileHead_.INTERVAL_MS)     );
    printf("INTERVAL_MS_ORI      :%hi \n", toLitteEnd(BFileHead_.INTERVAL_MS_ORI )    );
    printf("NUM_OF_SAMPLES_ORI   :%hi \n", toLitteEnd(BFileHead_.NUM_OF_SAMPLES_ORI ) );
    printf("SAMPLE_FORMAT        :%hi \n", toLitteEnd(BFileHead_.SAMPLE_FORMAT )  );
    printf("ENSEMBLE             :%hi \n", toLitteEnd(BFileHead_.ENSEMBLE   ) );
    printf("TRACE_SORT           :%hi \n", toLitteEnd(BFileHead_.TRACE_SORT ) );
    printf("VERT_SUM             :%hi \n", toLitteEnd(BFileHead_.VERT_SUM   ) );
    printf("SWEEP_FREQ_START     :%hi \n", toLitteEnd(BFileHead_.SWEEP_FREQ_START  )  );
    printf("SWEEP_FREQ_END       :%hi \n", toLitteEnd(BFileHead_.SWEEP_FREQ_END ) );
    printf("SWEEP_LENGTH         :%hi \n", toLitteEnd(BFileHead_.SWEEP_LENGTH   ) );
    printf("SWEEP_TYPE           :%hi \n", toLitteEnd(BFileHead_.SWEEP_TYPE ) );
    printf("SWEEP_NUM_CHANNEL    :%hi \n", toLitteEnd(BFileHead_.SWEEP_NUM_CHANNEL )  );
    printf("SWEEP_TAPER_LEN_START:%hi \n", toLitteEnd(BFileHead_.SWEEP_TAPER_LEN_START )  );
    printf("SWEEP_TAPER_LEN_END  :%hi \n", toLitteEnd(BFileHead_.SWEEP_TAPER_LEN_END   )  );
    printf("TAPER_TYPE           :%hi \n", toLitteEnd(BFileHead_.TAPER_TYPE ) );
    printf("CORRELATED           :%hi \n", toLitteEnd(BFileHead_.CORRELATED ) );
    printf("BINARY_GAIN          :%hi \n", toLitteEnd(BFileHead_.BINARY_GAIN )    );
    printf("AMP_RECOR            :%hi \n", toLitteEnd(BFileHead_.AMP_RECOR  ) );
    printf("MEASURE_SYSTEM       :%hi \n", toLitteEnd(BFileHead_.MEASURE_SYSTEM ) );
    printf("IMPULSE_POLAR        :%hi \n", toLitteEnd(BFileHead_.IMPULSE_POLAR  ) );
    printf("POLAR_CODE           :%hi \n", toLitteEnd(BFileHead_.POLAR_CODE ) );
    printf("SEGY_REV_NUM         :%hi \n", toLitteEnd(BFileHead_.SEGY_REV_NUM  )  );
    printf("FIXED_LEN            :%hi \n", toLitteEnd(BFileHead_.FIXED_LEN  ) );
    printf("NUM_EXT_HEAD         :%hi \n", toLitteEnd(BFileHead_.NUM_EXT_HEAD  )  );
}

void segy::PrintTextHeader(){
    for (size_t i = 0; i < 3200; i++)
	{
        if((i%80) == 0)										//80字节一组
            std::cout << std::endl;     
	    std::cout << e2a[(int) (TFileHead_[i])];			//读表法
	} 
    std::cout << std::endl;
}

void segy::getBuf(const char* const src,
					 char* const &buf,
					 const int &offset,
					 const int &len)
{
	int index;
	for (index = 0; index < len; index++)
		buf[index] = src[offset + index];
		switchByte(buf, len);
	return;
}

/*
工作站的大端模式变成PC小端模式
*/
void segy::switchByte(char* const c,const int &len)  
{
	int index;char tmp;
	for (index = 0; index < len / 2; index++)
	{
		tmp = c[index];
		c[index] = c[len - index - 1];
		c[len - index - 1] = tmp;
	}
}
/*
颜色映射函数
将数据标准化到0-1之间，然后在红-白-蓝三种三种颜色间映射
*/
void segy::colorMap()  
{
	auto it =intensity.begin();
	float value;
	for (it;it<intensity.end();it++)
	{
		if (*it>0)																	//蓝色
		{
			value = 1-*it/(float)maxValue;
			color->push_back(osg::Vec4f(value,value,1.0f,1.0f)); 
		}
		else																		//红色
		{
			value = 1-(-*it)/(float)maxValue;
			color->push_back(osg::Vec4f(1.0f,value,value,1.0f)); 
		}
	}
	for (int i = 0; i < 8; i++)														//上下平面
	{
		color->push_back(osg::Vec4f(1.0f,1.0f,1.0f,1.0f)); 
	}
	
}
/*
需要绘制的面数据读取
*/
void segy::readFaceData()
{
	if (InLine != 0)																//InLine!=0 体数据   InLine = 0 面数据
	{
		int flag = 0;
		int index = 0;
		float back = 0.0;
		float fvalue;  
		while(flag < 2)																// 正面与背面顶点赋值
		{
			flag++;		
			for (int i = 0; i < traceNum; i++)
			{
				for (int j = 0; j < sampleNum; j++)
				{
					fvalue = VolumeTrace[index][i][j];								//当index为0时表示正面 index为InLine-1为背面
					if (maxValue<=fvalue)
					{
						maxValue = fvalue;
					}
					data->push_back(osg::Vec3f(i/(float)traceNum,back,sampleNum/(float)traceNum - j/(float)traceNum));
					intensity.push_back(fvalue);
				}
			}
			index = InLine-1;														//最后一层
			back = InLine/(float)traceNum;
		}
		flag = 0;
		index = traceNum-1;
		back = 1;
		while (flag<2)																//左右
		{
			flag++;
			for (int i = 0; i < InLine; i++)
			{
				for (int j = 0; j < sampleNum; j++)
				{     
					fvalue = VolumeTrace[i][index][j];								//index = 0 表示左 index = traceNum-1 表示右
					if (maxValue<=fvalue)
					{
						maxValue = fvalue;
					}
					data->push_back(osg::Vec3f(back,i/(float)traceNum,sampleNum/(float)traceNum-j/(float)traceNum));
					intensity.push_back(fvalue);
				}
			}
			index = 0; 
			back = 0;
		}
		back = 0.0;
		while (flag>0)															//上下空白平面
		{
			data->push_back(osg::Vec3f(0.0f,0.0f,back));
			data->push_back(osg::Vec3f(0.0f,InLine/(float)traceNum,back));
			data->push_back(osg::Vec3f(1.0f,InLine/(float)traceNum,back));
			data->push_back(osg::Vec3f(1.0f,0.0f,back));
			flag--;
			back = sampleNum/(float)traceNum;
		}
	}
	else if(InLine == 0)														// 面数据
	{
		int fvalue = 0;
		for (int i = 0; i < traceNum; i++)
		{
			for (int j = 0; j < sampleNum; j++)
			{
				fvalue = Trace_[i][j];
				if (maxValue<=fvalue)
				{
					maxValue = fvalue;
				}
				data->push_back(osg::Vec3f(i/(float)traceNum,0,sampleNum/traceNum - j/(float)traceNum));
				intensity.push_back(fvalue);
			}
		}
	}
	else
	{
		std::cout<<"数据错误。";
		exit(0);
	}
}

/*
设置绘制图元
*/
void segy::setUnitGeom(osg::ref_ptr<osg::Geometry> geom)
{
	int time = traceNum-1;									//控制循环次数
	int span = 0;
	int flag = 0;
	if (InLine != 0)
	{
		while (flag<4)										//0--前面 1 后面 2上面 3下面
		{
			for (int i = 0; i < time; i++)
			{
				int pand = sampleNum*i;
				for (int j = pand+span; j < sampleNum-1+pand+span; j++)
				{
					osg::DrawElementsUInt* base = new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS,0);
					base->push_back(j);
					base->push_back(j+sampleNum);
					base->push_back(j+1+sampleNum);
					base->push_back(j+1);
					geom->addPrimitiveSet(base);
				}
			}
			if (++flag == 1)								//后面面片数据位置（需要跳过的长度）
			{
				span = traceNum*sampleNum;
			}
			else if(flag == 2)								//上面
			{
				time = InLine-1;
				span *= 2;
			}
			else if(flag == 3)								//下面
			{
				span += InLine*sampleNum;
			}
		}
		span += InLine*sampleNum;							//左右平面
		flag = 0;
		while (flag<2)
		{
			osg::DrawElementsUInt* base = new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS,0);
			for (int i = 0; i < 4; i++)
			{		
				base->push_back(span++);
			}
			geom->addPrimitiveSet(base);
			flag++;
		}
	}
	else if (InLine == 0)
	{
		for (int i = 0; i < time; i++)
		{
			int pand = sampleNum*i;
			for (int j = pand; j < sampleNum-1+pand; j++)
			{
				osg::DrawElementsUInt* base = new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS,0);
				base->push_back(j);
				base->push_back(j+sampleNum);
				base->push_back(j+1+sampleNum);
				base->push_back(j+1);
				geom->addPrimitiveSet(base);
			}
		}
	}
}

void segy::drawAllTrace()
{
	readFaceData();											//体数据
	colorMap();												//颜色映射
} 

void segy::outputFile()
{
	ofstream out("out.txt");
	out<<"626";
	out<<"\t";
	out<<"601";
	out<<"\t";
	out<<"301";
	out<<"\n";
	for (int i = 0; i < 300; i++)
	{
		for (int j = 0; j < 601; j++)
		{
			for (int k = 0; k < 626; k++)
			{
				if (out.is_open())   
				{  
					out << VolumeTrace[i][j][k];  
					out << "\t";    
				}  
			}
			out<<"\n";
		}
		out<<"\n";
	}
	out.close();
	cout<<"写文件结束";
}