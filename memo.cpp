// 競技プログラミング用メモ：よく使う操作と短いスニペット
// このファイルには汎用的なメモを集め、長い実装は snippets/ に分ける。
// 各見出しは独立した例。必要なコードだけコピーして使う。
// main関数を含む例やひな形を並べており、ファイル全体のコンパイルは想定しない。
//
// 目次（見出し番号・名前で検索）
//   00. 共通設定                     型・マクロ・定数
//   01. STL・文字列・入出力           冒頭にあった操作メモ
//   02. 短い数値・文字列の関数        ランレングス圧縮・整数平方根・篩・累乗
//   03. 区間・累積和・尺取り法        区間判定・1次元/2次元累積和・尺取り
//   04. ACLの短い使用例              Union-Find・modint
//   05. 考え方・実装時の注意          解法・数え上げ・グラフなどのメモ
//
// 分野別ファイル（すべて snippets/ 直下）
//   snippets/number_theory.cpp        素数関連ライブラリ・組合せ
//   snippets/data_structures.cpp      セグ木・遅延セグ木・トライ木・区間set
//   snippets/strings.cpp              ローリングハッシュ
//   snippets/graphs.cpp               BFS・ダイクストラ・木・木DP・パス・SCC
//   snippets/dp.cpp                   部分文字列DP・bitDP・TSP・桁DP
//   snippets/geometry.cpp             整数座標の内積・外積・直線・位置関係・交点・偏角ソート・角度判定

// ============================================================================
// 00. 共通設定
// ============================================================================
#include <atcoder/all>
using namespace atcoder;
using mint = modint998244353;
#include<bits/stdc++.h>
using namespace std;
using u64 = unsigned long long;
using u128 = unsigned __int128;
using i128 = __int128;
#define ll long long
#define ld long double
#define rep(i,n) for(ll i=0;i<n;i++)
#define drep(i,n) for(ll i=n-1;i>=0;i--)
#define nfor(i,s,n) for(ll i=s;i<n;i++)
#define vc(h,w) vector<vector<char>>vc(h,vector<char>(w))
#define fi first
#define se second
#define mp make_pair
#define pb push_back
#define endl "\n"
#define nall(v) v.begin(),v.end()
#define rall(v) v.rbegin(),v.rend()
#define pcnt(x) __builtin_popcountll(x) 
#define next_p(v) next_permutation(v.begin(),v.end())
const ll INF = 1LL << 61;
const int di[8]={-1, 1, 0, 0,1,1,-1,-1}; 
const int dj[8]={0, 0, -1, 1,-1,1,-1,1}; 
const char dc[4]={'U','D','L','R'};
void No() {cout <<"No"<<endl;exit(0);}
void Yes() {cout <<"Yes"<< endl;exit(0);}
template<class T> using minpq = priority_queue<T, vector<T>, greater<T>>;
template <class T, class U> bool chmin(T& current, const U& candidate) {
    if (current <= candidate) return false;
    current = candidate;      return true;
}
template <class T, class U> bool chmax(T& current, const U& candidate) {
    if (current >= candidate) return false;
    current = candidate;      return true;
}
bool in_grid(ll i, ll j, ll h, ll w) {//trueなら内部
    return ((0<=i && i<h && 0<=j &&j<w));
}
template<class T> void pv(const T& a){//配列を空白区切りで出力
    int f=0;
    for(const auto& x:a) cout<<(f++?" ":"")<<x;
    cout<<'\n';
}
template<class T> void pv2(const T& a){for(const auto& x:a) pv(x);}

//ll op(ll a, ll b) { return min(a, b); }
//ll e() { return INF; }

int main(){
  ios::sync_with_stdio(false);
  cin.tie(nullptr);
  
}


// ============================================================================
// 01. STL・文字列・入出力
// ============================================================================

// ソート・二分探索（探索する配列は昇順にソート済みであること）
// 大きい順にソートさせる比較関数 auto comp = [](int a, int b) { return a > b; };
// sort(d.begin(),d.end(),[](int p, int q) { return c[p]>c[q]; });　添字配列dをcの値の降順でソート
// sort(v.begin(), v.end(), greater<ll>());    sort(b.rbegin(), b.rend());で降順にsort
// auto itr=lower_bound(v.begin(),v.end(),x); x以上の最小のイテレータを返す
// upper_bound(v.begin(), v.end(), x) - v.begin()　は、「x 以下の要素数」(lower_boundにすればx未満の要素数)
// 重複削除　v.erase(unique(v.begin(), v.end()), v.end());
// uniqueは隣接する重複だけを消す。全体の重複削除には先にsortする。

// vector・set・map・tuple
// 配列の要素の削除a.erase(a.begin()+i);
// 配列の任意の位置に代入v.insert(v.begin()+i,a)  (位置,値)
// *st.rbegin(),*st.begin()でsetの最大値、最小値を取得
// vector<ll>v(n);  iota(v.begin(),v.end(),0);  v={0,1,2,,,n-1}
// iota(v.begin(),v.end(),0); 0から始まる連続値を設定
// tuple   get<0>(t)でtの0番目にアクセス
// mapはm.count(i)で存在を確かめないとキーが増えてしまう
// vector<ll> vec(s.begin(), s.end());でsetからvectorなどに変換できる
// 配列のサイズを後から与えるv.assign(n, 0);　(サイズ,初期値)　 二次元配列とか：v.assign(n, vector<ll>());

// 文字列・文字変換
// substr　文字列の切り抜き　substr(開始位置, 取り出す長さ);長さを省略すると最後まで切り抜ける
// string(i,'.'); .をi個並べた文字列
// 小文字、大文字判定　isupper(),islower(),小文字、大文字変換tolower();toupper()
// reverse(ans.begin(), ans.end()); string逆順
// ll ctol(char c){return c-'0';}charを数値に変換
// char ltoc(ll x){return char('0'+x);}数値をcharに変換

// 優先度付きキュー（小さい順）
// priority_queue<pair<ll,ll>,vector<pair<ll,ll>>,greater<pair<ll,ll>>>q;
// priority_queue<tuple<ll,ll,ll>, vector<tuple<ll,ll,ll>>, greater<>>q;
// 値が小さい順に取り出されるpriority_queueの宣言:priority_queue<型, vector<型>, greater<型>> 変数名;

// bit・全探索・順列
// bit全探索　for(ll p=0;p<(1<<n);p++){bitset<20>b(p);}  位置の指定は配列と逆で右から
// bit演算ではオーバーフローしないよう、1LL<<60のように、ll型を明示する
// do{処理内容}while(next_permutation(v.begin(),v.end()));       vの順列全列挙
// __builtin_popcount(i)でiの1bitの数を返す

// 入出力・数値・その他の記法
// ll inf=9223372036854775807;
// 回転vector<ll>x(4);x={1,0,-1,0};vector<ll>y(4);y={0,1,0,-1};
// cout<<fixed<<setprecision(n)<<ans<<endl;  下n桁まで出力
// hypot(x,y)=sqrt(x*x+y*y)
// 条件式 ? 真の時の式 : 偽の時の式
// hypot(x2 - x1, y2 - y1);ユークリッド距離
// 平方根を取るときはsqrt(static_cast<long double>(n));のようにしないと誤差が出る
// 負の値の余りを [0, mod) に直すときは (x % mod + mod) % mod（mod > 0、加算のオーバーフローに注意）

// ============================================================================
// 02. 短い数値・文字列の関数
// ============================================================================

// ---- 02-01. ランレングス圧縮 ----
// 連続する同じ文字を {文字, 個数} に圧縮。
// 前提: sは空でない。空文字列を扱う場合は先頭で空のvectorを返す。
vector<pair<char, ll>> rle(string s) {
    vector<pair<char, ll>> vec;
    int cnt = 1;
    for (int i = 1; i < (ll)s.size(); i++) {
        if (s[i] != s[i - 1]) {
            vec.push_back({s[i - 1], cnt});
            cnt = 0;
        }
        cnt++;
    }
    vec.push_back({s.back(), cnt});
    return vec;
}

// ---- 02-02. 整数平方根 ----
// 前提: x >= 0。y*y と (y+1)*(y+1) がllの範囲を超えない入力で使う。
// llの上限付近を扱う場合は、積を __int128 にして比較する。
ll f(ll x) {        //平方根√xを超えない最大の数を正しく返す
    ll y = sqrt(x); // ① まず double sqrt を信じてみる
    while (y * y > x)
        y--; // ② 大きすぎたら下げる
    while ((y + 1) * (y + 1) <= x)
        y++; // ③ 小さすぎたら上げる
    return y;
}

// ---- 02-03. エラトステネスの篩 ----
// 前提: n >= 1。n以下の素数を昇順で返す。計算量 O(n log log n)。
vector<ll> sieve(ll n) {
    vector<bool> is(n + 1, true);
    vector<ll> p;

    is[0] = is[1] = false;

    for (ll i = 2; i <= n; i++) {
        if (!is[i])
            continue;

        p.push_back(i);

        if (i * i <= n) {
            for (ll j = i * i; j <= n; j += i) {
                is[j] = false;
            }
        }
    }
    return p;
}

// ---- 02-04. 累乗(通常のpowだと誤差が出る) ----
// 前提: y >= 0。途中の re*x と x*x もllの範囲に収まること。
ll llpow(ll x, ll y) { //x^y
    ll re = 1;
    while (y > 0) {
        if (y & 1)
            re *= x; //yの最下位bitが1なら掛ける
        x *= x;
        y >>= 1; //yを1bit右へ
    }
    return re;
}

// ============================================================================
// 03. 区間・累積和・尺取り法
// ============================================================================

// ---- 03-01. 区間の交差 ----
// 半開区間 [l, r) の交差。端点が接するだけならfalse。
bool crossing(pair<ll, ll> p, pair<ll, ll> q) { //区間の交差を検出
    if (max(p.fi, q.fi) < min(p.se, q.se))
        return true;
    return false;
}

// 1次元累積和
// 累積和はsum[i] = 区間 [0, i) の総和  ,sum[i+1]=sum[i]+v[i]のように初期化
// 区間[l,r]の数え上げ:(rまでの個数)-(l-1までの個数):s[r+1]-s[l]
// 区間[l,r)の数え上げ:(r-1までの個数)-(l-1までの個数)s[r]-s[l]

// ---- 03-02. 累積和上の二分探索(区間 [l, r) の和 ≥ X を満たす最小 r) ----
// 前提: 要素が非負で累積和Sが単調非減少。
// r >= l に限定する場合は lower_bound(S.begin() + l, S.end(), S[l] + X)。
// S[r] - S[l] >= X ⇔ S[r] >= S[l] + X
auto it = lower_bound(S.begin(), S.end(), S[l] + X);

// ---- 03-03. 二次元累積和 ----
// sは (h+1)×(w+1) を0初期化。sumは [h1, h2)×[w1, w2) の和を返す。
rep(i, h) rep(j, w) s[i + 1][j + 1] = s[i + 1][j] + v[i][j];
rep(i, h) rep(j, w) s[i + 1][j + 1] += s[i][j + 1];
ll sum(ll h1, ll h2, ll w1, ll w2, vector<vector<ll>> const &s) {
    return s[h2][w2] - s[h1][w2] - s[h2][w1] + s[h1][w1];
}

// ---- 03-04. 尺取りテンプレ（和を管理） ----
// 【ひな形】条件OK・条件NGは問題に合わせて置き換える。
// 区間を伸ばすと条件が悪化するなど、尺取りできる単調性が必要。
ll r = 0, sum = 0, ans = 0;
rep(l, n) {
    while (r < n && 条件OK) {
        sum += a[r];
        if (条件NG) {
            sum -= a[r];
            break;
        }
        r++;
    }

    // 得られた区間での何かしらの処理
    ans += r - l;

    // を進める際の調節
    sum -= a[l];
    if (r < l + 1)
        r = l + 1;
}

// ---- 03-05. 尺取りテンプレ（汎用） ----
// 【ひな形】ok / add / solve / remove を問題に合わせて定義する。
// ok(r)は a[r] を追加してよいかを判定する。
// 半開区間 [l, r) を尺取りする
ll r = 0;
for (ll l = 0; l < n; ++l) {

    // r を伸ばせるだけ伸ばす
    while (r < n && ok(r)) {
        add(a[r]);
        ++r;
    }

    // この時点で [l, r) は条件を満たす最大区間
    solve(l, r);

    // l を 1 つ進める
    if (l == r) {
        // 空区間なら r も進める
        ++r;
    } else {
        remove(a[l]);
    }
}

// ============================================================================
// 04. ACLの短い使用例
// ============================================================================

// ---- 04-01. Union-Find(dsu) ----
// 必要: atcoder/dsu。頂点番号は0-indexed。
// クエリ: 0 b c は結合、それ以外は連結判定（1 / 0を出力）。
// main関数の中に貼る使用例。
ll n, q;
cin >> n >> q;
dsu d(n);
while (q--) {
    ll a, b, c;
    cin >> a >> b >> c;
    if (a == 0) {
        d.merge(b, c);
    } else {
        if (d.same(b, c))
            cout << 1 << endl;
        else
            cout << 0 << endl;
    }
}

// ---- 04-02. modint ----
// 必要: mint = atcoder::modint998244353。整数nをn回連結した値の計算例。
// inv()を取る値が0でないことが前提。0になる入力は別に扱う。
int main() {
    ll n;
    cin >> n;
    ll p = to_string(n).size();

    mint m = mint(mint(10).pow(n)).pow(p);
    m--;
    m *= mint(n);
    mint tmp = mint(10).pow(p);
    tmp--;
    mint k = tmp.inv();
    m *= k;
    cout << m.val() << endl;
}

// ============================================================================
// 05. 考え方・実装時の注意
// ============================================================================

// 方針・数え上げ
// クエリの処理が多く無駄な差分が多いときは、漸化式を考えて答えを逆算する(「1回前の状態を考えて追う」を繰り返す)
// 回文数列挙　上半分の桁をループさせて折り返すだけ　7桁,8桁の場合は1000~9999を折り返す
// 方針を変えるときは今の方針の実装自体に問題がないか確認する
// 配列から条件を満たす集合の数え方の一つ、1点を固定し、(左の選び方 + 1) × (右の選び方 + 1)でカウント
// 部分文字列から条件を満たす[l,r]のカウントの一つ、dpで右側を動かしていく(既存の集合に新しい部分文字列)
// 数字を繋げた文字列の列挙は既存のものの末尾に追加のみで対応できるか考える
// xorではbitごとに独立に問題が解けないか注目する
// 自明な上界や下界を考える

// グラフ
// 状態遷移付きダイクストラではseenを状態別に管理する
// ダイクストラでは隣接リストを見る前に現在地が既に格納されている値より大きくないか確認するべき

// bitDPは snippets/dp.cpp に
