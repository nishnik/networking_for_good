
#include <iostream>
#include <stdlib.h>
#include <thread>
#include <string>
#include <fstream>
using namespace std;

int work_division_check=0;
#define verbose_level 1
void send_cap(string cap_path)
{

}
void send_bssid(string bssid)
{

}
pair<bool,string> send_part(string s)
{
	return make_pair(0,"garbage");
}
void del_files()
{
	string del_files="rm -r part*";
	system(del_files.c_str());
#if verbose_level>0
	cout<<"\n Extra Files Destroyed";
#endif
}
pair<bool,string> run_client_side(const string& cap_file,const string& wordlist,const string& bssid="")
{
	string s = "aircrack-ng -q " + cap_file + " -w " + wordlist ;
	if(bssid.length()!=0)
		s+=" -b " + bssid;
#if verbose_level>0
	else
		cout<<"\n Running without a bssid";
#endif
	s+= " > result_"+bssid+".abhi"; 
#if verbose_level>0
	cout << "Executing: " << s <<'\n';
#endif
	int ret = system( s.c_str() );
#if verbose_level>0
	cout << "Executed: " << s <<'\n';
	cout << "aircrack returned: "<< ret <<'\n'; 
#endif
	if(ret!=0 )
		return make_pair(0,"ERROR_AIRCRACK_FAILED");
	string s_t = "result_"+bssid+".abhi";
	ifstream f(s_t.c_str());//
	if(f.eof())
		return make_pair(0,"ERROR_FILE_OPEN_1");
#if verbose_level>1
	else
		cout << " File opened\n";
#endif
	getline(f,s_t);
#if verbose_level>1
	cout << s_t << '\n';
#endif
	int found = s_t.find("[ ");
	if(found == string::npos)
		return  make_pair(0,"AIRCRACK_PASS_NOT_FOUND");

	else
	{
		int end = s_t.find(" ]");
		return make_pair( 1 ,  s_t.substr(13, s_t.length()-15));
	}
/*#if verbose_level>0
	cout << t;// <<;
#endif
	if( t == "Passphrase")
		return  make_pair(0,"AIRCRACK_PASS_NOT_FOUND");
	else if (t == "KEY")
	{
		f >> t;
		cout << t ;
		while(f!="[")
			f >> t;
		f >> t;
		cout << t ;
		return make_pair(1,t);
	}
*/	
	return make_pair(0,"UNHANDELED_EXCEPTION");
}

void client_side()
{
	
	while(work_division_check<90 )
	{
		string s="part";
		int j=work_division_check++;
		if(j<10)
			s=s+"0"+to_string(j);
		else
			s=s+to_string(j);
		pair<bool,string> res = send_part(s);
		
#if verbose_level>0
		cout<<"\n"<<s<<" in client\n";
		cout<<"\n"<<res.second<<"\n";
#endif
		if(res.first==1)
		{
			cout<<"\n KEY Found: "<<res.second;
			del_files();
			exit(0);
		}		
	}
}

void server_side(const string &cap_path,const string &bssid)
{
	while(work_division_check<90)
	{

		string s="part";
		int j=work_division_check++;
		if(j<10)
			s=s+"0"+to_string(j);
		else
			s=s+to_string(j);
		pair<bool,string> res = run_client_side(cap_path,s,bssid);
#if verbose_level>0
		cout<<"\n"<<s<<" in server\n";
		cout<<"\n"<<res.second<<"\n";
#endif
		if(res.first==1)
		{
			cout<<"\n KEY Found: "<<res.second;
			del_files();
			exit(0);
		}
	}
}
void work_division(const string &cap_path,const string &bssid)
{
	thread first (server_side,cap_path,bssid);
	thread second (client_side);
	first.join();
	second.join();
}
void file_creation(const string &dict_path)
{
	
		string argument_split;
		argument_split="split -n 90 -d "+dict_path+" part";
		system(argument_split.c_str());
#if verbose_level>0
		cout<<"\n Files created";
#endif	
}

int main()
{
	cout<<"\n Enter the address of dictionary file w.r.t. current dir: ";
	
	string dict_path;
	getline(cin,dict_path);
	
	string argument_if_exist="[ ! -e "+dict_path+" ];";
	int result_if_exist=system(argument_if_exist.c_str());
	
	if(result_if_exist==256)
	{
		cout<<"\n Enter the address of capture file w.r.t. current dir: ";
		
		string cap_path;
		getline(cin,cap_path);
		
		string argument_if_exist="[ ! -e "+cap_path+" ];";
		result_if_exist=system(argument_if_exist.c_str());
		
		if(result_if_exist==256)
		{
			file_creation(dict_path);
			send_cap(cap_path);
			cout<<"\n Enter the BSSID['n' if you dont want to enter]: ";
			string bssid;
			getline(cin,bssid);
			send_bssid(bssid);
			if(bssid!="n")
				work_division(cap_path,bssid);
			else
				work_division(cap_path,"");
			
		
		}
		else
		{
			cout<<"\n Capture File doesnt exist";		
		}	
	}
	else
	{
		cout<<"\n Dictionary File doesnt exist";
	}
	del_files();

	
}
