#include "hexsearch.h"


Hexsearch::Hexsearch(const SearchArgType &args,uint64_t startpos,uint64_t endpos):args_(args),pos_(startpos),startpos_(startpos),endpos_(endpos)
{
	ReparseArgs();
}
Hexsearch::~Hexsearch()
{

}
inline bool Hexsearch::Compare(int a,int b)
{
	if(b==Hexsearch::ANYBYTE)
		return true;
	if(a==b)
		return true;
	return false;
}
void Hexsearch::ReparseArgs()
{
	SearchArgType targ;
	targ=args_;
	args_.clear();
	for(size_t i=0;i<targ.size();)
	{
		if(targ[i]==DWORD_)
		{
			i++;
			for(int j=0;j<4;j++)
			{
				args_.push_back((targ[i]&(0xff<<i))>>i);//TODO is this correct???

			}
			i++;
		}
		else if (targ[i]==ANYDWORD)
		{
			i++;
			for(int j=0;j<4;j++)
				args_.push_back(ANYBYTE);
		}
		else
		{
			args_.push_back(targ[i]);
			i++;
		}
	}
}
uint64_t Hexsearch::FindNext() //TODO rewrite using Boyer-Moore algorithm
{
	DFHack::Core &inst=DFHack::Core::getInstance();
	DFHack::Process *p=inst.p;
	uint8_t *buf;
	buf=new uint8_t[args_.size()];
	while(pos_<endpos_)
	{
		bool found=true;
		p->readByte(pos_,buf[0]);
		if(Compare(buf[0],args_[0])) 
		{
			p->read(pos_,args_.size(),buf);
			for(size_t i=0;i<args_.size();i++)
			{
				if(!Compare(buf[i],args_[i]))
				{
					pos_+=i;
					found=false;
					break;
				}
			}
			if(found)
			{
				pos_+=args_.size();
				delete [] buf;
				return pos_-args_.size();
			}
		}
		pos_++;
	}
	delete [] buf;
	return 0;
}

std::vector<uint64_t> Hexsearch::FindAll()
{
	std::vector<uint64_t> ret;
	uint64_t cpos=pos_;
	while(cpos!=0)
	{
		cpos=FindNext();
		if(cpos!=0)
			ret.push_back(cpos);
	}
	return ret;
}
void Hexsearch::PrepareBadCharShift()
{
	BadCharShifts.resize(256,-1);
	int i=0;
	for(SearchArgType::reverse_iterator it=args_.rbegin();it!=args_.rend();it++)
	{
		BadCharShifts[*it]=i;
		i++;
	}
}
void Hexsearch::PrepareGoodSuffixTable()
{
	GoodSuffixShift.resize(args_.size()+1,0);

}