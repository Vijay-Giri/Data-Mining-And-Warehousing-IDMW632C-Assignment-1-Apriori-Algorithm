#include <bits/stdc++.h>
#include <fstream>
using namespace std;

vector < set < int > > trans;   //This vector contains the details of each transaction where index of vector is transaction id and set is its correponding itemset.
vector < map < set <int> , int> > passes;   // This vector contains each of the k-itemset generated 
int MAX;  
int min_sup = 3000;     //minimum support count
void setClosedFrequent();   
void display();
int convert(string &x)  //this function finds the integer value from the string of number
{
    int ans = 0;
    for(int i = 0; i < x.size(); i++)
    {
        ans = ans*10 + (x[i] - '0');
    }
    return ans;
}

string convert2(int x)
{
    string ans = "";
    while(x)
    {
        ans += (x%10 + '0');
        x = x/10;
    }
    reverse(ans.begin(), ans.end());
    return ans;
}

void preprocess(string line)  //This function preprocesses each line of the file to get itemset of corresponding transaction id
{
    string x = "";     //empty string
    set < int > temp;  //set to store the itemset of the given transaction id
    for(int i = 0; i < line.size(); i++)
    {
        if(line[i] == ' ')
	{
            temp.insert(convert(x));    // we are inserting the item in the itemset one by one
            MAX = max(MAX, convert(x)); //we are finding MAX to know what is the maximum items there in a any transaction in entire dataset which will help us in later stage to scan through the dataset
            x = "";    // reinitializing x to empty string
        }
        else x += line[i];   // concatenating/adding the character until a space is encountered ...this gives us items for the given transaction
    }
    temp.insert(convert(x)); //again we insert the last item to the itemset
    MAX = max(MAX, convert(x)); // finding max
    trans.push_back(temp);  // finally we push this to our transaction vector which contains itemset for corresponding transaction id
}

map < set <int>, int > items;   // This map contains k-itemset with its support count
void process()     // This function performs the Apriori Algorithm
{
    for(int i = 1; i <= MAX; i++)   // This is done to obtain the support count of every items by scanning through the entire transaction dataset
    {
        int cnt = 0;
        set < int > s;
        s.insert(i);
        for(int j = 0; j < trans.size(); j++)  //scanning through the entire dataset for each item to find support count
	{
            if(trans[j].find(i) != trans[j].end())
	    {
                cnt++;
            }
        }
        if(cnt >= min_sup)    //if support count is >= threshold min support count then we insert it in itemset else discard it
	{
            items[s] = cnt;
        }
    }
    map < set <int> , int > :: iterator it, it3;  // iterators 
    passes.push_back(items);    //pushing L1 to vector passes 
    while(1)   // we run this loop until (k+1)-itemset is an empty set
    {
        map < set <int> , int > naya;  // this map stores the (k+1)-itemset and its support count
        for(it = items.begin(); it != items.end(); it++)
	{
            it3 = ++it;    //if it points to i-th row then it3 points to the (i+1)-th row ....they are used to find (k+1)-itemset using k-itemset
            it--;
            for(; it3 != items.end(); it3++)
	    {
                set < int > s1 = (it->first), s2 = (it3->first);   
                set < int > s3;     // the set s3 stores the (k+1)-itemset
                set < int > :: iterator it1 = s1.begin(), it2 = s2.begin();
                int flag = 0;
                for(int i = 0; i < s1.size()-1; i++)
		{
                    if(*it1 != *it2)    // to make (k+1)-itemset from k-itemset we have to make sure that k-1 initial items should to be equal otherwise they wont contribute to (k+1) itemset formation
		    {
                        flag = 1;
                        break;
                    }
                    s3.insert(*it1);
                    it1++;
                    it2++;
                }
                if(flag == 0)  // Once (k+1)-itemset is prepared we check if Apriori Property is satisfied or not...if not then we discard it otherswise include it in (k+1)-itemset
		{
                    int flag1 = 0;
                    s3.insert(*it1);
                    s3.insert(*it2);
                    it1 = s3.begin();
                    int sz = s3.size();
                    set < int > s4 = s3;  
                    for(int i = 0; i < sz; i++) 
		    {
                        int tmp = *it1;
                        s4.erase(tmp);
                        if(items.find(s4) == items.end())   // checking if every subset is present in k-itemset or not
			{
                            flag1 = 1;
                            break;
                        }
                        s4.insert(tmp);  
                        it1++;
                    }
                    if(flag1 == 0)   // now if apriori property is satisfied we check whether support count is >= min_sup or not
		    {
                        int cnt = 0;
                        for(int i = 0; i < trans.size(); i++)   //scanning through the dataset to find support count
			{
                            int flag2 = 0;
                            for(it1 = s3.begin(); it1 != s3.end(); it1++)
			    {
                                if(trans[i].find(*it1) == trans[i].end()){
                                    flag2 = 1;
                                    break;
                                }
                            }
                            if(flag2 == 0)
			    {
                                cnt++;
                            }
                        }
                        if(cnt >= min_sup) naya[s3] = cnt;   //if support count is >=min_sup we insert it in map naya 
                    }
                }
            }
        }
        if(naya.size() == 0)  //if naya is empty that means (k+1)-itemset is empty ..which means it is end of Apriori algorithm and we break!
	{
            break;
        }
        else  //else we push this map to our vector passes
	{
            items = naya;
            passes.push_back(items);
        }
    }
    setClosedFrequent();   // calling setClosedFrequent() function to find Closed Frequent itemset / Closed Itemset
}
map < set < int >, string > closedFrequentOrNot;   // this map stores detail about whether or not a itemset is Closed Frequent itemset / closed itemset
void setClosedFrequent()
{
	for(int i = 0; i < passes.size(); i++)   //revisiting all tables to find Closed Frequent and Closed itemset
	{
		if(i == passes.size()-1) //last table always contain Closed Frequent Itemset because their superset is not possible
		{
			map < set < int >, int > m1 = passes[i];
			for(auto it = m1.begin(); it != m1.end(); it++)
			{
				set < int > s = it->first;
				closedFrequentOrNot[s] = "Closed Frequent Itemset";
			}
			continue;
		}

		map < set < int >, int > m1 = passes[i], m2 = passes[i+1]; // map m1 stores k-itemset and map m2 stores (k+1)-itemset
		for(auto it = m1.begin(); it != m1.end(); it++)
		{
			int flag = 0;
			set < int > s1 = it->first;
			for(auto it2 = m2.begin(); it2 != m2.end(); it2++)  //scanning through the superset to compare the support counts
			{
				set < int > s2 = it2->first;
				if(includes(s2.begin(), s2.end(), s1.begin(), s1.end()))
				{
					if(it->second == it2->second)   // is support count of a set and its superset is equal then it is a closed itemset
					{
						flag = 1;
						closedFrequentOrNot[s1] = "Closed Itemset";
						break;
					}
				}
			}
			if(flag == 0)  // if support count is not equal to its superset then it is a Closed Frequent Itemset
			{
				closedFrequentOrNot[s1] = "Closed Frequent Itemset";
			}	
		}
	}
	display();  //calling display() function to print the obtained results
}

void display()   //This function prints every tables
{
	for(int i = 0; i < passes.size(); i++)
	{
		cout << "Pass " << i+1 << endl;
		map < set < int >, int >  m = passes[i];
		for(auto it = m.begin(); it != m.end(); it++)
		{
			set < int > s = it->first;
			for(auto it1 = s.begin(); it1 != s.end(); it1++)
			{
				cout << *it1 << " ";
			}
			cout << "--> " << it->second << " --> " << closedFrequentOrNot[s] << endl;
		}
		cout << endl;
	}
}

int main(int argc, char * argv[])
{
    ifstream myfile(argv[1]);
    if(!myfile.is_open())    
        exit(EXIT_FAILURE);

    string line;
    while ((getline(myfile, line)))   //reading line one by one from the file
    {
        preprocess(line);
    }
    process();
    myfile.close();
}
