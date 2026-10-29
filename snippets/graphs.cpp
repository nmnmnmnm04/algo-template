// グラフ：BFS・ダイクストラ・木・木DP・パス・SCC
// 用途に合う見出しから必要なコードだけコピーして使うコードメモ。
// 各ブロックは独立した例。ファイル全体をまとめて実行するためのものではない。
// 共通の型・マクロは ../memo.cpp の「00. 共通設定」を参照。
// 目次: 01. BFS / 02. ダイクストラ / 03. Tree / 04. 木DP / 05. 直径 / 06. 親配列・パス / 07. SCC

// ============================================================================
// 01. grid上のBFSでの最短経路
// ============================================================================

// ---- 4近傍・壁は # ----
// 必要: ll, INF, rep, fi, se, di, dj, in_grid。
// vは空でなく、s,gは範囲内の通行可能なマス。distはh×wをINF初期化。
ll bfs(pair<ll, ll> s, pair<ll, ll> g, const vector<string> &v, vector<vector<ll>> &dist) {
    const ll h = v.size();
    const ll w = v[0].size();
    dist[s.fi][s.se] = 0;
    if (s == g)
        return 0;
    queue<pair<ll, ll>> q;

    q.push(s);
    while (!q.empty()) {
        auto [x, y] = q.front();
        ll d = dist[x][y];
        q.pop();
        rep(k, 4) {
            ll nx = x + di[k];
            ll ny = y + dj[k];
            if (in_grid(nx, ny, h, w)) {
                if (v[nx][ny] != '#' && dist[nx][ny] > d + 1) {
                    dist[nx][ny] = d + 1;
                    if (g == make_pair(nx, ny)) {
                        return d + 1;
                    }
                    q.push({nx, ny});
                }
            }
        }
    }
    return INF;
}

// ============================================================================
// 02. 普通のダイクストラ
// ============================================================================

// ---- 非負重みの無向グラフ ----
// 必要: ll, INF, rep, nfor, pb。始点は頂点0、入力の頂点番号は1-indexed。
// 辺の重みは非負。到達可能な距離はINF未満であること。
int main() {
    ll n, m;
    cin >> n >> m;
    vector v(n, vector<pair<ll, ll>>());
    rep(i, m) {
        ll a, b, t;
        cin >> a >> b >> t;
        a--;
        b--;
        v[a].pb(make_pair(b, t));
        v[b].pb(make_pair(a, t));
    }

    vector<ll> dist(n, INF);
    priority_queue<pair<ll, ll>, vector<pair<ll, ll>>, greater<pair<ll, ll>>> q;
    dist[0] = 0;
    q.push(make_pair(0, 0));

    while (!q.empty()) {
        auto [d, id] = q.top();
        q.pop();
        if (d > dist[id])
            continue;
        for (auto [nv, cost] : v[id]) {
            ll nc = d + cost;
            if (nc < dist[nv]) {
                dist[nv] = nc;
                q.push({dist[nv], nv});
            }
        }
    }

    nfor(i, 1, n) {
        cout << dist[i] << " ";
    }
    cout << "" << endl;
}

// ============================================================================
// 03. 木構造(連結な単純無向グラフで与えられる前提)
// ============================================================================

// ---- Tree本体 ----
// 必要: ll, pb。再帰DFSなので深い木ではスタックの上限に注意。
// 後続の木DP・直径の例でもこのTreeを使う。
// init()は木専用。閉路のあるグラフには直接使わず、先に全域木を取り出す。
// init()後のparは、06の tree_path(u, v, t.par) にそのまま渡せる。
// t.g[now]	隣接頂点と辺の重み
// t.sz[now]	自身を含む部分木サイズ
// t.par[now]	親頂点
// t.dep[now]	根からの辺数
// t.dist[now]	根からの重み付き距離
struct Tree {
    struct Edge {
        ll to, w;
    };
    ll n;
    vector<vector<Edge>> g;
    vector<ll> sz, par, dep, dist;

    Tree(ll n = 0) {
        assign(n);
    }

    void assign(ll n_) {
        n = n_;
        g.assign(n, {});
        sz.assign(n, 0);
        par.assign(n, -1);
        dep.assign(n, 0);
        dist.assign(n, 0);
    }

    void add(ll a, ll b, ll w = 1) {
        g[a].pb({b, w});
        g[b].pb({a, w});
    }

    void dfs(ll now, ll p) {
        par[now] = p;
        sz[now] = 1;
        for (auto [to, w] : g[now]) {
            if (to == p)
                continue;
            dep[to] = dep[now] + 1;
            dist[to] = dist[now] + w;
            dfs(to, now);
            sz[now] += sz[to];
        }
    }

    void init(ll root = 0) {
        dep[root] = dist[root] = 0;
        dfs(root, -1);
    }
};

// ---- 使用例 ----
// 必要: Tree, ll, rep, pv。
Tree tree;
int main() {
    ll n;
    cin >> n;
    tree.assign(n);
    rep(i, n - 1) {
        ll a, b, w;
        cin >> a >> b >> w;
        tree.add(--a, --b, w);
        // 重みなしなら tree.add(a,b);
    }
    tree.init();

    pv(tree.sz);
    pv(tree.par);
    pv(tree.dep);
    pv(tree.dist);
}

// ============================================================================
// 04. 木DP
// ============================================================================

// ---- 04-01. 木DP：親 → 子（行きがけ） ----
// 【ひな形】Tree t(n) と dp(n) は初期化の例。
// 実際にはt,dpをdfsから参照できるスコープに置き、木を構築してからdfs(root)を呼ぶ。
Tree t(n);
vector<ll> dp(n);

void dfs(ll now, ll p = -1) {
    for (auto [to, w] : t.g[now]) { //for (auto [to,w]:v[now])
        if (to == p)
            continue;
        dp[to] = dp[now] + w; // 親から子へ伝搬
        dfs(to, now);
    }
}

// ---- 04-02. 木DP：子 → 親（帰りがけ） ----
// 【ひな形】Treeとdpの配置・初期化は04-01と同じ。
Tree t(n);
vector<ll> dp(n);

ll dfs(ll now, ll p = -1) {
    ll res = 1; // 自身の初期値

    for (auto [to, w] : t.g[now]) { //for (auto [to,w]:v[now])
        if (to == p)
            continue;
        res += dfs(to, now); // 子の結果を集約
    }

    return dp[now] = res;
}

// ---- 04-03. 木DP：子 → 親（帰りがけ）／重みなし隣接リスト版 ----
// 必要: ll。gとdpを頂点数分だけ初期化してからdfs(root)を呼ぶ。
vector<vector<ll>> g;
vector<ll> dp;

ll dfs(ll now, ll p = -1) {
    ll res = 1; // 自身の初期値
    for (ll to : g[now]) {
        if (to == p)
            continue;
        res += dfs(to, now); // 子の結果を集約
    }
    return dp[now] = res;
}

// ============================================================================
// 05. 木の直径（辺の重みは非負）
// ============================================================================

// ---- 子方向の最長2経路を集約 ----
// 【ひな形】必要: Tree, ll, chmax。Treeとtの配置・初期化は04-01と同じ。
// ans=0に初期化してdfs(root)を呼ぶ。
Tree t(n);
ll ans = 0;

// nowから子方向へ伸ばせる最長経路長を返す
ll dfs(ll now, ll p = -1) {
    ll mx1 = 0, mx2 = 0;

    for (auto [to, w] : t.g[now]) {
        if (to == p)
            continue;

        ll x = dfs(to, now) + w;

        if (x >= mx1)
            mx2 = mx1, mx1 = x;
        else if (x > mx2)
            mx2 = x;
    }

    chmax(ans, mx1 + mx2);
    return mx1;
}

// ============================================================================
// 06. 親配列の作成・木上のuvパス取得
// ============================================================================

// 木そのものが入力される場合: Treeに辺を追加 → t.init(root) → t.par を使う。
// 一般のグラフから全域木を作る場合: 06-03のbfs_parentで親配列を作る。
// 自分で探索する場合: 頂点nxを初めて発見したとき parent[nx] = now と記録する。
// どの場合も同じtree_pathを使える。根の親は -1 に統一する。

// ---- 06-01. tree_path ----
// 必要: ll, pb, nall。u,vは同じ木の頂点。根の親は -1 にする。
// bfs_parentの結果を渡す場合は、u,vがどちらも探索済み（parが -2 でない）こと。
// 計算量は O(頂点数)（根までの親をたどる）。
// 探索時に親を記録したparent配列が必要となる
vector<ll> tree_path(ll u, ll v, const vector<ll> &par) {
    vector<ll> a, b;
    for (; u != -1; u = par[u])
        a.pb(u);
    for (; v != -1; v = par[v])
        b.pb(v);

    // 共通部分をLCAだけ残して削る
    while (a.size() > 1 && b.size() > 1 && a[a.size() - 2] == b[b.size() - 2]) {
        a.pop_back();
        b.pop_back();
    }

    b.pop_back(); // LCAの重複を消す
    reverse(nall(b));
    for (ll x : b)
        a.pb(x);

    return a;
}

// ---- 06-02. 木が入力される場合（Treeと組み合わせる） ----
// 必要: 03のTree、06-01のtree_path、ll, rep, pv。
// 入力: n u v、その後にn-1本の辺a b。頂点番号は入力・出力ともに1-indexed。
int main() {
    ll n, u, v;
    cin >> n >> u >> v;
    --u;
    --v;

    Tree t(n);
    rep(i, n - 1) {
        ll a, b;
        cin >> a >> b;
        t.add(a - 1, b - 1);
    }
    t.init(0); // 根は任意。u-vパスは根の選び方によらない。

    auto path = tree_path(u, v, t.par);
    for (ll &x : path)
        ++x; // 出力用に1-indexedへ戻す
    pv(path);
}

// ---- 06-03. 一般のグラフから全域木の親配列を作る（BFS） ----
// 必要: ll。gは重みなし無向グラフの隣接リスト。0 <= root < g.size()。
// 戻り値: 根の親は -1、未到達の頂点は -2、それ以外は親頂点の番号。
// 連結なら全域木、非連結ならrootの連結成分だけの木になる。計算量 O(V + E)。
// 閉路があっても、初めて発見した頂点だけを木に加える。
// 得られるのはBFS木上のパス。任意のu-v間で元のグラフの最短路になるとは限らない。
vector<ll> bfs_parent(const vector<vector<ll>> &g, ll root = 0) {
    vector<ll> par(g.size(), -2);
    queue<ll> q;
    par[root] = -1;
    q.push(root);

    while (!q.empty()) {
        ll now = q.front();
        q.pop();
        for (ll to : g[now]) {
            if (par[to] != -2)
                continue;
            par[to] = now;
            q.push(to);
        }
    }
    return par;
}

// ---- 06-04. 全域木上のパスを取得する使用例 ----
// 必要: 06-01のtree_path、06-03のbfs_parent、ll, rep, pb, pv。
// 入力: n m u v、その後にm本の辺a b。頂点番号は入力・出力ともに1-indexed。
// ここでは頂点uを根にするので、vに到達できればu-v間の最短路にもなる。
int main() {
    ll n, m, u, v;
    cin >> n >> m >> u >> v;
    --u;
    --v;
    vector<vector<ll>> g(n);
    rep(i, m) {
        ll a, b;
        cin >> a >> b;
        g[a - 1].pb(b - 1);
        g[b - 1].pb(a - 1);
    }

    auto par = bfs_parent(g, u);
    if (par[v] == -2) {
        cout << -1 << '\n'; // uとvが別の連結成分にある
        return 0;
    }
    auto path = tree_path(u, v, par);
    for (ll &x : path)
        ++x;
    pv(path);
}

// 06-05. 実際に使用した例（ABC472E）
// この例では探索と同時に色seenと親parentを記録し、tree_pathに渡している。

// ============================================================================
// 07. SCC（強連結成分分解・ACL）
// ============================================================================

// 必要: atcoder/scc（またはatcoder/all）, using namespace atcoder, rep。
// 頂点は0-indexed。計算量 O(n + m)。
scc_graph g(n);
g.add_edge(u, v); // 有向辺u→v（辺をすべて追加してからscc()）
auto groups = g.scc(); // vector<vector<int>>。groups[i]は成分iの頂点一覧

// 成分はトポロジカル順。異なる成分への辺u→vではid[u] < id[v]。
// 有向閉路あり ⇔ 頂点数2以上の成分がある、または自己ループがある。
// 自己ループだけならサイズ1のSCCになるため、自己ループは辺の追加時などに別途確認する。
