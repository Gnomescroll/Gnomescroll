#include <iostream>
#include <vector>
#include <queue>

#define FOR(X,Y,Z) for(int X=Y; X<=Z; X++)
#define MAXN 203

#define pb push_back
#define mp make_pair
#define vi(X) vector< X >::iterator
using namespace std;

int N, M, skojarzenia[MAXN], d[MAXN];
vector<int>graf[MAXN];

vector<vector<int> >paths;
vector<int>gm[MAXN];
vector<int>gmp[MAXN];
vector<int>p;

int visited[203];


bool DFS(int v) {
    p.pb(v);
    visited[v]=1;
    if( v>M && !skojarzenia[v] ) return true;

    for( vi(int) i=gmp[v].begin(); i!=gmp[v].end(); i++) 
        if( !visited[*i] && DFS(*i) ) return true;

    visited[v]=0;
    p.pop_back();
    return false;
}

bool MaxPaths() {
    paths.clear();

    // wartosci poczatkowe O(V)

    FOR(i,1,M+N) {
        visited[i]=0;
        d[i]=1000;
        gm[i].clear();
        gmp[i].clear();
    }
    FOR(i,1,M)  if( !skojarzenia[i] ) d[i]=0;

    /* tworzenie grafu skojarzenia gm O(E) */
    FOR(i,1,M)
    for( vi(int) j=graf[i].begin(); j!=graf[i].end(); j++ )
        skojarzenia[i]==*j ? gm[*j].pb(i) : gm[i].pb(*j);

    /* obliczenie odleglosci od wierzcholkow do V1' O(E) */
    FOR(i,1,M) {
        if( skojarzenia[i] ) { continue; }
        queue<int>Q; Q.push(i);
        while( !Q.empty() ) {
            int v=Q.front(); Q.pop();
            for( vi(int) j=gm[v].begin(); j!=gm[v].end(); j++) {
                if( d[*j] > d[v]+1 ) {
                    d[*j]=d[v]+1;
                    Q.push(*j);
                }
            }
        }   // so ugly ;]
    }
    FOR(i,1,M+N) // usuwanie krawedzi niepotrzebnych O(E)
    for( vi(int) j=gm[i].begin(); j!=gm[i].end(); j++) {
        if( d[i]+1 == d[*j] ) gmp[i].pb(*j);
    } 
    
    FOR(i,1,M) if( !skojarzenia[i] ) {
        p.clear();
        if( DFS(i) ) {
            paths.pb( p );
            break;
        }
    }
    if( !paths.empty() ) return true;
    return false;
}

int main() {
    //ios_base::sync_with_stdio(false);
    //cin.tie(0);
    while(cin >> N) {
        cin >> M;
        FOR(i,1,N+M) {
            graf[i].clear();
            skojarzenia[i]=0;
        }
        int k, w;
        FOR(i,1,M) {    // wczytaj graf
            cin >> k;
            FOR(j,1,k) {
                cin >> w;
                graf[i].pb(w+M);
                graf[w+M].pb(i);
            }
        }
        while( MaxPaths() ) {   // dopoki sa rosnace sciezki powiekszajace
            for(vi(vector<int>) i=paths.begin(); i!=paths.end(); i++)
            for(vi(int) j=i->begin(); j!=i->end(); j+=2) {
                skojarzenia[ skojarzenia[*j] ] = 0;
                skojarzenia[*j] = *(j+1);
                skojarzenia[ skojarzenia[*(j+1)] ] = 0;
                skojarzenia[*(j+1)] = *j;
            }                   
        }
        int ile=0;
        FOR(i,1,M) if( skojarzenia[i] ) ile++;
        cout << ile << endl;
        // wypluj wynik \n
    }

    return 0;
}
