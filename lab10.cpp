#include<iostream>
#include<fstream>
#include<iterator>
#include<vector>
#include<string>
#include<pthread.h>
#include<ncurses.h>
#include<cstdlib>
#include<locale>
#include<algorithm>
#include<cctype>
#include<iomanip>
#include<openssl/md5.h>
#include<cmath>
#include<sstream>
#include<signal.h>

using namespace std;

#define MAX_THREADS 10
#define MAX_NUMBER 100

struct decoded {
  string entered_pass;
  string decoded_pass;
  friend ostream &operator<<(ostream &oStr, const decoded &item);
};

void read_file(vector<string> &vec, string sName);
void read_hash(vector<string> &vec, string sName);
void toLowerLetter(string *pass);
void toUpperLetter(string *pass);
void firstToUpper(string *pass);
void* lowLetterProduce(void* arg);
void* firstGreatProduce(void* arg);
void* GreatLettersProduce(void* arg);
void* lowLetterProduce2(void* arg);
void* firstGreatProduce2(void* arg);
void* GreatLettersProduce2(void* arg);
void* consume(void *);
void find_pass(string *pass);
void print_list(vector<decoded> &vec);
void print_list(vector<string> &vec);
string md5(string *pass);
unsigned GetNumberOfDigits(int i);
void findPassWithNumber(string *pass);
void handler(int sig_num);

vector<decoded> dec_list;
vector<string> hash;
vector<string> dictionary;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int pass_counter;
int all_pass;


int main(int argc, char **argv)
{
  string temp;
  pthread_t tid_produce[6];
  pthread_t tid_consume;
  int count[MAX_THREADS];
  string hashName;
  int nthreads = 6;

  read_file(dictionary, "English.txt");
  pthread_setconcurrency(nthreads);

  for(;;)
    {
      cout << "Podaj nazwe pliku w ktorym znajduje sie haslo do zlamania" << endl;
      cin >> hashName;
      read_hash(hash, hashName);
      all_pass = hash.size();
      pass_counter = 0;
  
      pthread_create(&tid_produce[0], NULL, lowLetterProduce, &count[0]);
      pthread_create(&tid_produce[1], NULL, lowLetterProduce2, &count[1]);
      pthread_create(&tid_produce[2], NULL, firstGreatProduce, &count[2]);
      pthread_create(&tid_produce[3], NULL, firstGreatProduce2, &count[3]);
      pthread_create(&tid_produce[4], NULL, GreatLettersProduce, &count[4]);
      pthread_create(&tid_produce[5], NULL, GreatLettersProduce2, &count[5]);


      pthread_create(&tid_consume, NULL, consume, NULL);
      for(int i = 0; i < nthreads; i++)
	{
	  pthread_join(tid_produce[i], NULL);
      
	}
      pthread_join(tid_consume, NULL);
    }
  getch();
  return 0;
}

void* lowLetterProduce(void* arg)
{
  string numb;
  for(vector<string>::iterator it = dictionary.begin(); it != dictionary.end(); it++)
    {
      pthread_mutex_lock(&mutex);
      string temp = *it;
      toLowerLetter(&temp);
      find_pass(&temp);
      findPassWithNumber(&temp);
      pthread_mutex_unlock(&mutex);
    }
}

void* lowLetterProduce2(void* arg)
{
  for(vector<string>::iterator it = dictionary.begin(); it != dictionary.end(); it++)
    for(vector<string>::iterator it_2 = dictionary.begin(); it_2 != dictionary.end(); it_2++)
      {
	pthread_mutex_lock(&mutex);
	string temp = *it + *it_2;
	toLowerLetter(&temp);
	find_pass(&temp);
	findPassWithNumber(&temp);
	pthread_mutex_unlock(&mutex);
      }
}

void* firstGreatProduce(void* arg)
{
  for(vector<string>::iterator it = dictionary.begin(); it != dictionary.end(); it++)
    {
      pthread_mutex_lock(&mutex);
      string temp = *it;
      firstToUpper(&temp);
      find_pass(&temp);
      findPassWithNumber(&temp);
      pthread_mutex_unlock(&mutex);
    }
}

void* firstGreatProduce2(void* arg)
{
  for(vector<string>::iterator it = dictionary.begin(); it != dictionary.end(); it++)
    for(vector<string>::iterator it_2 = dictionary.begin(); it_2 != dictionary.end(); it_2++)
      {
	pthread_mutex_lock(&mutex);
	string temp = *it + *it_2;
	firstToUpper(&temp);
	find_pass(&temp);
	findPassWithNumber(&temp);
	pthread_mutex_unlock(&mutex);
      }
}

void* GreatLettersProduce(void* arg)
{
  for(vector<string>::iterator it = dictionary.begin(); it != dictionary.end(); it++)
    {
      pthread_mutex_lock(&mutex);
      string temp = *it;
      toUpperLetter(&temp);
      find_pass(&temp);
      findPassWithNumber(&temp);
      pthread_mutex_unlock(&mutex);
    }
}

void* GreatLettersProduce2(void* arg)
{
  for(vector<string>::iterator it = dictionary.begin(); it != dictionary.end(); it++)
    for(vector<string>::iterator it_2 = dictionary.begin(); it_2 != dictionary.end(); it_2++)
      {
	pthread_mutex_lock(&mutex);
	string temp = *it + *it_2;
	toUpperLetter(&temp);
	find_pass(&temp);
	findPassWithNumber(&temp);
	pthread_mutex_unlock(&mutex);
      }
}

void* consume(void *arg)
{
  while(1)
    {
      pthread_mutex_lock(&mutex);
      print_list(dec_list);
      pthread_mutex_unlock(&mutex);
      signal(SIGHUP, handler);
      if(hash.empty())
	break;
    }
}

void read_file(vector<string> &vec, string sName)
{

  ifstream fName;
  fName.open(sName.c_str(), std::ifstream::in);
  istream_iterator<string> begin(fName);
  istream_iterator<string> end;
  vector<string> temp_vec(begin, end);
  vec = temp_vec;
  fName.close();
}


void read_hash(vector<string> &vec, string sName)
{
  vector<string> temp;
  read_file(temp, sName);
  for(vector<string>::iterator it = temp.begin(); it != temp.end(); it++)
    {
      if((*it).size() == 32 && (*it).find_first_not_of("0123456789abcdefABCDEF") == -1)
	{
	  vec.push_back(*it);
	}
    }
}

void toLowerLetter(string *pass)
{
  std::locale loc;
  for(std::string::iterator it = (*pass).begin(); it != (*pass).end(); it++)
    {
      *it = std::tolower(*it, loc);
    }
}

void toUpperLetter(string *pass)
{
  std::locale loc;
  for(std::string::iterator it = (*pass).begin(); it != (*pass).end(); it++)
    {
      *it = std::toupper(*it, loc);
    }
}
void firstToUpper(string *pass)
{
  toLowerLetter(pass);
  (*pass)[0] = toupper((*pass)[0]);
}


ostream &operator<< (ostream &oStr, const decoded &item)
{
  return oStr << setw(35) << item.entered_pass
	      << "---->" << setw(35) << item.decoded_pass;
}


void find_pass(string *pass)
{
  if(!hash.empty())
    {
      for(vector<string>::iterator it = hash.begin(); it != hash.end(); it++)
	{
	  if(md5(&(*pass)) == (*it))
	    {
	      decoded temp;
	      temp.entered_pass = *pass;
	      temp.decoded_pass = *it;
	      dec_list.push_back(temp);
	      hash.erase(it);
	      pass_counter++;
	      return;
	    }
	}
    }
}

void print_list(vector<decoded> &vec)
{
  if(!vec.empty())
    {
      for(vector<decoded>::iterator it = vec.begin(); it != vec.end(); it++)
	cout << *it << endl;
      vec.clear();
    }
}
void print_list(vector<string> &vec)
{
  if(!vec.empty())
    {
      for(vector<string>::iterator it = vec.begin(); it != vec.end(); it++)
	cout << *it << endl;
      vec.clear();
    }
}

string md5(string *pass)
{
  string retWord;
  unsigned char pass_temp[50];
  MD5((unsigned char*)(*pass).c_str(), (*pass).length(), (unsigned char*)&pass_temp);
  
  char mdString[32];

  for(int i = 0; i < 16; i++)
    {
      sprintf(&mdString[i*2], "%02x", (unsigned int)pass_temp[i]);
    }
  return string(mdString);
}

unsigned GetNumberOfDigits(int i)
{
  return i > 0 ? (int)log10((double)i) + 1: 1;
}

void findPassWithNumber(string *pass)
{
  stringstream sStr;
      for(int i = 0; i < MAX_NUMBER; i++)
	{
	  sStr << i;
	  string numb = (*pass) + sStr.str();
	  find_pass(&numb);
	  numb = sStr.str() + (*pass);
	  find_pass(&numb);
	  for(int j = 0; j < 100; j++)
	    {
	      if(GetNumberOfDigits(i) == GetNumberOfDigits(j))
		{
		  sStr.str("");
		  sStr << j;
		  string dubNumb = numb + sStr.str();
		  find_pass(&dubNumb);
		}
	      else if(GetNumberOfDigits(i) < GetNumberOfDigits(j))
		break;
	    }
	  sStr.str("");
	}
}

void handler(int sig_num)
{
  cout << "All passwords: " << all_pass << endl;
  cout << "Broken ones: " << pass_counter << endl;
  cout << "Leftovers to break: " << all_pass - pass_counter << endl;
}
