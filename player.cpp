#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
using namespace std;

const int STATES = 1<<13, SCORE = 64, LEGAL = 462, PERM = 7*7*7*7*7;
const double MODF = 1.0/6.0;

vector<int> legal, mask[6], back[LEGAL], forw[LEGAL];
int ID[PERM];
int choose[13][LEGAL];

bool valid(int, int);
void preProcessing();
vector<int> toVector(int);
int toPerm(vector<int>);
int calc(int, int);
void computeMasks(int, int);
void solve(int);
void solve1(int, int);
void solve2(int, int);
void solve3(int, int);

double exp[STATES][SCORE];
double lExp[LEGAL][SCORE][3];
int bestChoice[LEGAL][SCORE][3];

int main()
{
    preProcessing();
    
    ifstream inputfile;
    inputfile.open("solution");
    
    for(int i=0; i<STATES; ++i)
        for(int j=0; j<SCORE; ++j)
            inputfile >> exp[i][j];
    
    inputfile.close();
    
    int state = 0, score = 0, total = 0;
    for(int round=0; round<13; ++round)
    {
        solve3(state, score);
        solve2(state, score);
        solve1(state, score);
        
        cout << "What did you roll?" << "\n";
        
        vector<int> v(5);
        for(int i=0; i<5; ++i)
            cin >> v[i];
        sort(v.begin(), v.end());
        
        int roll = ID[toPerm(v)];
        int choice = bestChoice[roll][score][0];
        
        v = toVector(legal[choice]);
        for(int i=0; i<5; ++i)
            cout << v[i] << " ";
        cout << "\n";
        
        cout << "What did you roll?" << "\n";
        
        for(int i=0; i<5; ++i)
            cin >> v[i];
        sort(v.begin(), v.end());
        
        roll = ID[toPerm(v)];
        choice = bestChoice[roll][score][1];
        
        v = toVector(legal[choice]);
        for(int i=0; i<5; ++i)
            cout << v[i] << " ";
        cout << "\n";
        
        cout << "What did you roll?" << "\n";
        
        for(int i=0; i<5; ++i)
            cin >> v[i];
        sort(v.begin(), v.end());
        
        roll = ID[toPerm(v)];
        choice = bestChoice[roll][score][2];
        
        cout << "Score " << choice+1 << "\n";
        
        state = state|(1<<choice);
        
        if(choice < 6)
        {
            int newscore = score + choose[choice][roll];
            newscore = min(newscore, 63);
            
            if(newscore==63 && score<63)
                total += 35;
            
            score = newscore;
        }
        total += choose[choice][roll];
        cout << "Current Total: " << total << "\n";
    }
}

bool valid(int state, int l)
{
    int r = 0;
    for(int b=0; b<13; ++b)
        r += (bool)((1<<b)&(state));
    
    return (r==l);
}

void preProcessing()
{
    int size = PERM;
    
    for(int i=0; i<size; ++i)
    {
        vector<int> value = toVector(i);
        
        bool incorrect = false;
        int zeros = 0;
        
        for(int j=1; j<5; ++j)
        {
            if(value[j] < value[j-1])
            {
                incorrect = true;
                break;
            }
        }
        
        for(int j=0; j<5; ++j)
        {
            if(value[j] == 0)
            {
                zeros++;
            }
        }
        
        if(!incorrect)
        {
            ID[i] = legal.size();
            mask[zeros].push_back(ID[i]);
            legal.push_back(i);
        }
        else
            ID[i] = -1;
    }
    
    for(int i=0; i<legal.size(); ++i)
    {
        vector<int> value = toVector(legal[i]);
        
        if(value[0] == 0)
        {
            for(int v=1; v<=6; ++v)
            {
                vector<int> newvalue = value;
                newvalue[0] = v;
                sort(newvalue.begin(), newvalue.end());
                
                int roll = ID[toPerm(newvalue)];
                back[i].push_back(roll);
            }
        }
    }
    
    for(int i=0; i<legal.size(); ++i)
    {
        vector<int> value = toVector(legal[i]);
        
        vector<int> candidates;
        
        for(int replace=0; replace<(1<<5); ++replace)
        {
            vector<int> newvalue(value);
            for(int j=0; j<5; ++j)
            {
                if(replace&(1<<j))
                    newvalue[j] = 0;
            }
            
            sort(newvalue.begin(), newvalue.end());
            int roll = ID[toPerm(newvalue)];
            
            candidates.push_back(roll);
        }
        
        sort(candidates.begin(), candidates.end());
        candidates.erase(unique(candidates.begin(), candidates.end()), candidates.end());
        
        forw[i] = candidates;
    }
    
    for(int i=0; i<13; ++i)
        for(int j=0; j<LEGAL; ++j)
            choose[i][j] = calc(i, j);
}

vector<int> toVector(int num)
{
    vector<int> v(5);
    
    int i = 0;
    
    while(num)
    {
        v[i] = (num%7);
        num /= 7;
        
        i++;
    }
    
    return v;
}

int toPerm(vector<int> vec)
{
    int num = 0, b = 1;
    
    for(int i=0; i<5; ++i)
    {
        num += b * vec[i];
        b = b*7;
    }
    
    return num;
}

int calc(int choice, int roll)
{
    vector<int> vec = toVector(legal[roll]);
    
    int sum = 0, count[7] = { 0 };
    for(int i=0; i<5; ++i)
    {
        sum += vec[i];
        count[vec[i]]++;
    }
    
    int best = 0;
    int curr = 0;
    
    for(int i=1; i<7; ++i)
    {
        if(count[i] > 0)
        {
            curr++;
            best = max(curr, best);
        }
        else
            curr = 0;
    }
    
    
    if(choice < 6)
    {
        int modifier = choice+1;
        int count = 0;
        for(int i=0; i<5; ++i)
            if(vec[i] == modifier)
                count++;
        
        return modifier*count;
    }
    if(choice == 6)
    {
        for(int i=1; i<7; ++i)
            if(count[i] >= 3)
                return sum;
        return 0;
    }
    if(choice == 7)
    {
        for(int i=1; i<7; ++i)
            if(count[i] >= 4)
                return sum;
        return 0;
    }
    if(choice == 8)
    {
        int two = 0, three = 0;
        for(int i=1; i<7; ++i)
        {
            if(count[i] == 3)
                three++;
            else if(count[i] == 2)
                two++;
        }
        
        if(two && three)
            return 25;
        return 0;
    }
    if(choice == 9)
    {
        if(best >= 4)
            return 30;
        return 0;
    }
    if(choice == 10)
    {
        if(best >= 5)
            return 40;
        return 0;
    }
    if(choice == 11)
    {
        for(int i=1; i<7; ++i)
            if(count[i] >= 5)
                return 50;
        return 0;
    }
    if(choice == 12)
    {
        return sum;
    }
}

void computeMasks(int ind, int score)
{
    for(int i=1; i<6; ++i)
    {
        for(int j=0; j<mask[i].size(); ++j)
        {
            int cur = mask[i][j];
            double ans = 0;
            for(int e=0; e<back[cur].size(); ++e)
            {
                int dest = back[cur][e];
                ans += lExp[dest][score][ind];
            }
            ans *= MODF;
            
            lExp[cur][score][ind] = ans;
        }
    }
}

void solve(int state)
{
    for(int score = 0; score < SCORE; ++score)
    {
        solve3(state, score);
        solve2(state, score);
        solve1(state, score);
    }
}

void solve3(int state, int score)
{
    for(int i=0; i<mask[0].size(); ++i)
    {
        int v = mask[0][i];
        
        double profit[13], best = 0;
        
        for(int c=0; c<13; ++c)
        {
            profit[c] = 0;
            
            if((state&(1<<c)) == 0)
            {
                int newstate = state|(1<<c);
                int newscore = score;
                
                if(c<6)
                    newscore = score + choose[c][v];
                newscore = min(newscore, 63);
                
                if(score < 63 && newscore == 63)
                    profit[c] += 35;
                
                profit[c] += choose[c][v];
                profit[c] += exp[newstate][newscore];
            }
            
            if(profit[c] > best)
            {
                best = profit[c];
                bestChoice[v][score][2] = c;
            }
        }
        
        lExp[v][score][2] = best;
    }
    
    computeMasks(2, score);
}

void solve2(int state, int score)
{
    for(int i=0; i<mask[0].size(); ++i)
    {
        int v = mask[0][i];
        
        double best = 0;
        for(int e=0; e<forw[v].size(); ++e)
        {
            int dest = forw[v][e];
            
            if(lExp[dest][score][2] > best)
            {
                best = lExp[dest][score][2];
                bestChoice[v][score][1] = dest;
            }
        }
        
        lExp[v][score][1] = best;
    }
    
    computeMasks(1, score);
}

void solve1(int state, int score)
{
    for(int i=0; i<mask[0].size(); ++i)
    {
        int v = mask[0][i];
        
        double best = 0;
        for(int e=0; e<forw[v].size(); ++e)
        {
            int dest = forw[v][e];
            
            if(lExp[dest][score][1] > best)
            {
                best = lExp[dest][score][1];
                bestChoice[v][score][0] = dest;
            }
        }
        
        lExp[v][score][0] = best;
    }
    
    computeMasks(0, score);
}
