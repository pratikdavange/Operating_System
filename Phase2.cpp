#include<sstream>
#include<iostream>
#include<string.h>
#include<string>
#include<fstream>
#include<string>
#include<cstdlib>

using namespace std;

ifstream fin;
ofstream fout;
int SI,PI,TI;
bool occupied_pages[30];

class memory
{
private:
    char mem[300][4];
    char ch;
    int page_table_ptr;

public:
    void reset()
    {

        memset(mem,'*',sizeof(char)*300*4);
        memset(occupied_pages,false,sizeof(bool)*30);
        page_table_ptr=rand()%30;
        printf("%d\n",page_table_ptr);
        occupied_pages[page_table_ptr]=true;
        page_table_ptr*=10;
        printf("%d\n",page_table_ptr);
        SI=PI=TI=0;
    }
    string get_mem(int pos)
    {

        string temp="";
        for(int i=0;i<4;i++)
            temp+=mem[pos][i];
        //cout<<"String "<<temp<<endl;
        return temp;
    }
    void set_mem(string s, int pos)
    {
        for(int i=0;i<4;i++)
            mem[pos][i]=s[i];
    }
    int get_page_table_ptr()
    {
        return page_table_ptr;
    }
    int allocate_page()
    {
        int page_no=rand()%30;
        while(occupied_pages[page_no]==true)
            page_no=rand()%30;
        occupied_pages[page_no]=true;
        //cout<<"Page no: "<<page_no<<endl;
        return page_no;
    }
    void set_page_table(int row_num,int page_no)
    {
        ostringstream temp;
        temp << page_no;
        string table_entry;
        if(page_no<10)
            table_entry="**0"+temp.str();
        else
            table_entry="**"+temp.str();
        set_mem(table_entry,page_table_ptr+row_num);
    }
    void store_card(string s,int mem_cnt)
    {
        string word="";
        int page_no=allocate_page();
        printf("%d\n",page_no);
        set_page_table(mem_cnt, page_no);
        page_no*=10;
        for(int i=0;i<s.length();i+=4)
        {
            for(int j=0;j<4;j++)
            {
                word+=s[i+j];
            }
            set_mem(word,page_no);
            page_no++;
            //cout<<"Word: "<<word<<endl;
            word="";
        }
    }
    void print_mem()
    {
        for(int i=0;i<300;i++)
        {
            string temp;
            cout<<"Data at mem location"<<"["<<i<<"]";
            for(int j=0;j<4;j++)
            {
                cout<<mem[i][j];
            }
            cout<<"\n";

        }

    }
}m_obj;

class cpu
{
private:
    int fetched_IC,terminate_code,TLL,TTL,LLC,TLC,pos;
    bool terminate,fetched_C;
    string fetched_IR,operand,opreator,fetched_R,compare_string,p_id;
    char IR[4],R[4],IC[2];
    bool C,run_mos;

public:
    int s_to_i(string operand)
    {

        if(operand[0]>='0' && operand[0]<='9' && operand[1]>='0' && operand[1]<='9')
            return ((int)operand[0]-48)*10+((int)operand[1]-48);
        return -1;
    }
    void set_limits(string limits)
    {
        p_id=limits.substr(0,4);
        TTL=s_to_i(limits.substr(4,2))*100+s_to_i(limits.substr(6,2));
        TLL=s_to_i(limits.substr(8,2))*100+s_to_i(limits.substr(10,2));
        LLC=0;TLC=0;terminate_code=0;
    }
    void set_IC()
    {
        IC[0]='0';
        IC[1]='0';
    }
    void set_IC(int pos)
    {
        IC[1]=((char)pos%10)+48;
        pos=pos/10;
        IC[0]=((char)pos%10)+48;
    }
    int get_IC()
    {
        int val;
        val=((int)IC[0]-48)*10+((int)IC[1]-48);
        return val;
    }
    void inc_IC()
    {
        int val;
        val=get_IC();
        val++;
        set_IC(val);
    }

    void set_IR(int IC)
    {
        string returned_value="";
        returned_value=m_obj.get_mem(IC);
        for(int i=0;i<4;i++)
            IR[i]=returned_value[i];
    }
    string get_IR()
    {
        string ret_IR="";
        for(int i=0;i<4;i++)
            ret_IR+=IR[i];
        return ret_IR;
    }
    void set_R(int pos)
    {
        string returned_value="";
        returned_value=m_obj.get_mem(pos);
        for(int i=0;i<4;i++)
            R[i]=returned_value[i];
    }
    string get_R()
    {
        string ret_R="";
        for(int i=0;i<4;i++)
        {
            ret_R+=R[i];
        }
        return ret_R;
    }
    void set_C(bool value)
    {
        C=value;
    }
    bool get_C()
    {
        return C;
    }
    int address_tranlation(int virtual_add)
    {
        int page=m_obj.get_page_table_ptr()+(virtual_add/10);
        string value_page=m_obj.get_mem(page);
        if(value_page[1]=='$')
        {
            PI=3;
            return -1;
        }
        value_page=value_page.substr(2,2);
        return (s_to_i(value_page)*10+(virtual_add%10));
    }
    int address_tranlation(string op)
    {
        if(s_to_i(op)==-1)
        {
            PI=2;
            return -2;
        }
        else
            address_tranlation(s_to_i(op));
    }
    void startexe()
    {

        set_IC(0);
        terminate=false;
        while(!terminate)
        {

            run_mos=false;
            fetched_IC=address_tranlation(get_IC());
            inc_IC();
            set_IR(fetched_IC);
            fetched_IR=get_IR();
            if((fetched_IR.compare("H"))==3)
                fetched_IR="Hrrr";
            opreator=fetched_IR.substr(0,2);
            operand=fetched_IR.substr(2,2);
            pos=address_tranlation(s_to_i(operand));  //if pos  == -2 then operand error
            if(address_tranlation(operand)!=-2 || !(operand.compare("rr")))
            {
                if(!(opreator.compare("LR")))
                {

                    if(pos==-1) //pos = -1 means page fault
                    {
                        run_mos=true;
                    }
                    else
                    {
                        set_R(pos);
                        TLC++;
                    }
                }
                else if (!(opreator.compare("SR")))
                {

                    fetched_R=get_R();
                    if(pos==-1)
                    {
                        run_mos=true;
                    }
                    else
                    {
                        m_obj.set_mem(fetched_R, pos);
                        TLC+=2;
                    }
                }
                else if (!(opreator.compare("CR")))
                {

                    fetched_R=get_R();
                    if(pos==-1)
                    {
                        run_mos=true;
                    }
                    else
                    {
                        TLC++;
                        compare_string=m_obj.get_mem(pos);
                        if(fetched_R.compare(compare_string)==0)
                            set_C(true);
                        else
                            set_C(false);
                    }
                }
                else if (!(opreator.compare("BT")))
                {


                    fetched_C=get_C();
                    if(fetched_C)
                    {

                        set_IC(s_to_i(operand));
                    }
                    TLC++;
                }
                else if (!(opreator.compare("GD")))
                {
                    if(pos!=-1)
                    {
                        TLC+=2;
                        SI=1;
                    }
                    run_mos=true;
                }
                else if (!(opreator.compare("PD")))
                {
                    if(pos!=-1)
                    {
                        TLC++;
                        SI=2;
                    }
                    run_mos=true;
                }
                else if (!(opreator.compare("Hr")))
                {
                    TLC++;
                    fetched_IR="H";
                    SI=3;
                    run_mos=true;
                }
                else
                {
                    PI=1;
                    run_mos=true;
                }
            }
            else
            {
                run_mos=true;
            }

            if(TLC>TTL)
            {
                TI=2;
                run_mos=true;
                terminate=true;
                terminate_code=3;  //tle
            }
            if(run_mos)
                MOS();
        }
    }

    void MOS()
    {
        if(TI==0 && SI==1)
        {


            string s;
            pos=(pos/10)*10;
            getline(fin,s);
            if(s.find("$END")!=-1)
            {
                terminate=true;
                terminate_code=1;   //no data error
            }
            else
            {
                if(!s.empty() && s[s.size()-1]=='\r')
                    s.erase(s.size()-1);
                int len=s.length(),start=0,i;
                string s1;
                for(i=pos;start<len;i++)
                {
                    if((len-start)<4)
                        s1=s.substr(start,(len-start));
                    else
                        s1=s.substr(start,4);
                    start+=4;
                    m_obj.set_mem(s1,i);
                }
            }
            SI=0;
        }
        else if(TI==2 && SI==1)
        {
            terminate=true;
            terminate_code=3;
        }
        else if((TI==0 || TI==2) && SI==2)
        {
            LLC++;
            if(LLC>TLL)
            {
                terminate=true;
                terminate_code=2;  //Line limit exceeded
            }

            else
            {
                int pos=address_tranlation(s_to_i(operand)),flag=0;
                pos=(pos/10)*10;
                string ans="",temp="";
                for(int i=pos;i<pos+10;i++)
                {
                    temp=m_obj.get_mem(i);
                    for(int j=0;j<4;j++)
                    {
                        if(temp[j]=='\0' || temp[j]=='$')
                        {
                            break;
                            flag=1;
                        }
                        ans+=temp[j];
                    }
                    if(flag)
                        break;
                }
                fout<<ans<<endl;
                if(TI==2)
                {
                    terminate=true;
                    terminate_code=3;  //tle
                }
            }
            SI=0;
        }
        else if((TI==0 || TI==2) && SI==3)
        {


            terminate=true;
            terminate_code=0; //Halt and tle or notle
        }
        else if(TI==0 && PI==1)
        {
            terminate_code=4;  //opcode error
            terminate=true;
        }
        else if(TI==0 && PI==2)
        {
            terminate_code=5;  //operand error
            terminate=true;
        }

        else if(TI==0 && PI==3)
        {
            if(!(opreator.compare("GD")) || !(opreator.compare("SR")))
            {
                int page_no=m_obj.allocate_page();
                m_obj.set_page_table((s_to_i(operand))/10,page_no);
                set_IC(get_IC()-1);
            }
            else
            {
                terminate_code=6;  //Invalid page fault
                terminate=true;
            }
        }

        else if(TI==2 && PI==1)
        {
            terminate_code=7;
            terminate=true;
        }

        else if(TI==2 && PI==2)
        {
            terminate_code=8;
            terminate=true;
        }

        else if(TI==2 && PI==3)
        {
            terminate_code=3;
            terminate=true;
        }
        if(terminate)
        {
            fout<<p_id<<" ";
            switch(terminate_code)
            {
                case 0:fout<<"NO ERROR\n";
                    break;
                case 1:fout<<"OUT OF DATA\n";
                    TLC--;
                    break;
                case 2:fout<<"LINE LIMIT EXCEEDED\n";
                    break;
                case 3:fout<<"TIME LIMIT EXCEEDED\n";
                    TLC=TTL;
                    break;
                case 4:fout<<"OPERATION CODE ERROR\n";
                    break;
                case 5:fout<<"OPERAND ERROR\n";
                    break;
                case 6:fout<<"INVALID PAGE FAULT\n";
                    break;
                case 7:fout<<"TIME LIMIT EXCEEDED with OPERATION CODE ERROR\n";
                    break;
                case 8:fout<<"TIME LIMIT EXCEEDED with OPERAND ERROR\n";
                    break;
            }
            fout<<"IC "<<get_IC()<<"  INSTRUCTION "<<fetched_IR<<" TLC "<<TLC<<" LLC "<<LLC<<endl;
            fout<<endl<<endl;

        }
    }
}exe;

void load()
{
    string s,s1;
    int mem_cnt=0;
    while(!(fin.eof()))
    {


        getline(fin,s);
        if(s.empty())
            break;
        if(s.find("$AMJ")!=-1)
        {

            m_obj.reset();
            mem_cnt=0;
            exe.set_limits(s.substr(4,12));
            continue;
        }
        else if(s.find("$DTA")!=-1)
        {
            exe.startexe();
        }
        else if(s.find("$END")!=-1)
        {

            mem_cnt=0;
            continue;
        }
        else
        {

            m_obj.store_card(s,mem_cnt++);
        }
    }

}

int main()
{

    fin.open("input.txt");
    fout.open("output.txt");
    load();
    m_obj.print_mem();
    fin.close();
    fout.close();
    return 0;
}