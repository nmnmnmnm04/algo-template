// データ構造：セグ木・遅延セグ木・トライ木・区間set
// 用途に合う見出しから必要なコードだけコピーして使うコードメモ。
// 各ブロックは独立した例。ファイル全体をまとめて実行するためのものではない。
// 共通の型・マクロは ../memo.cpp の「00. 共通設定」を参照。
// ACLの例は <atcoder/all> と using namespace atcoder; が必要。
// 目次: 01. セグ木 / 02. 遅延セグ木 / 03. トライ木 / 04. 区間set

// ============================================================================
// 01. セグ木で区間最大と二分探索
// ============================================================================

// ---- 区間最大・max_right ----
// 必要: ll, INF, rep, atcoder::segtree。
// 入力の位置aは1-indexed、seg内部の添字は0-indexed。prodは半開区間。
// max_rightで使う述語は区間を伸ばしたとき単調で、f(e()) = true を満たすこと。
ll op(ll a, ll b) {
    return max(a, b);
}
ll e() {
    return -INF;
} //区間最小とかならINF,区間和は0,ここでバグらせないよう注意が必要

int main() {
    ll n, q;
    cin >> n >> q;
    vector<ll> v(n);
    rep(i, n) cin >> v[i];
    segtree<ll, op, e> seg(v);
    while (q--) {
        ll t, a, b;
        cin >> t >> a >> b;
        if (t == 1) {
            seg.set(a - 1, b); //位置a-1にbを代入
        } else if (t == 3) {
            ll ans = n;
            // 二分探索で条件を満たさない右端(満たす左端)を探す
            // f(e()) = trueを満たさないとだめ
            ans = seg.max_right(a - 1, [&b](ll x) { return x < b; }) + 1;
            cout << ans << endl;
        } else {
            // 区間最大
            cout << seg.prod(a - 1, b) << endl;
        }
    }
}

// ============================================================================
// 02. 遅延セグ木区間加算・区間最小
// ============================================================================

// ---- 文字の変更に応じた累積和の更新例 ----
// 必要: ll, INF, rep, atcoder::lazy_segtree。
// mapping(f,e()) = e() が必要な用途では、INFに加算しないmappingにする。
// https://betrue12.hateblo.jp/entry/2020/09/23/005940
ll op(ll a, ll b) {
    return min(a, b);
}
ll e() {
    return INF;
}
ll mapping(ll f, ll x) {
    return f + x;
}
ll composition(ll f, ll g) {
    return f + g;
}
ll id() {
    return 0;
}

int main() {
    ll n;
    cin >> n;
    string s;
    cin >> s;
    vector<ll> v(n + 1);
    rep(i, n) {
        if (s[i] == 'A') {
            v[i + 1] = v[i] + 1;
        } else
            v[i + 1] = v[i] - 1;
    }
    lazy_segtree<ll, op, e, ll, mapping, composition, id> lseg(v);

    ll q;
    cin >> q;
    while (q--) {
        ll t;
        cin >> t;
        if (t == 1) {
            ll x;
            cin >> x;
            x--;
            char c;
            cin >> c;
            if (c == s[x])
                continue;
            s[x] = c;
            if (c == 'A')
                lseg.apply(x + 1, n + 1, 2);
            else
                lseg.apply(x + 1, n + 1, -2);
        } else {
            ll l, r;
            cin >> l >> r;
            ll m = lseg.prod(l, r + 1);
            if (m < lseg.get(l - 1))
                cout << "No" << endl;
            else
                cout << "Yes" << endl;
        }
    }
}

// ============================================================================
// 03. トライ木
// ============================================================================

// ---- Trie本体 ----
// 空列は根 t[0] に対応する。元実装のcntは根では更新しないため、
// prefix_count(空列) は登録総数ではなく0を返す。
template <class T> struct Trie {
    struct Node {
        map<T, int> nex; // 次の値 -> 次のノード
        // 必要なら追加
        int cnt = 0;     // このnodeを通る列の個数
        int end = 0;     // このnodeで終わる列の個数
        vector<int> idx; // この列に対応するindexなど
    };

    vector<Node> t = vector<Node>(1); // t[0] = root = 空列

    // v から x を1個追加した先へ進む
    // 辺がなければ新しく作る
    int add(int v, const T &x) {
        if (!t[v].nex.count(x)) {
            t[v].nex[x] = t.size();
            t.push_back(Node{});
        }
        return t[v].nex[x];
    }

    // 列 s を追加し、終点のノード番号を返す
    template <class Seq> int insert(const Seq &s) {
        int v = 0;

        for (const auto &x : s) {
            v = add(v, x);
            t[v].cnt++;
        }

        t[v].end++;
        return v;
    }

    // 列 s に対応するノードを返す。存在しなければ -1
    template <class Seq> int find(const Seq &s) const {
        int v = 0;

        for (const auto &x : s) {
            auto it = t[v].nex.find(x);
            if (it == t[v].nex.end())
                return -1;
            v = it->second;
        }

        return v;
    }

    // s が何個登録されているか
    template <class Seq> int count(const Seq &s) const {
        int v = find(s);
        return v == -1 ? 0 : t[v].end;
    }

    // s をprefixに持つ列の個数
    template <class Seq> int prefix_count(const Seq &s) const {
        int v = find(s);
        return v == -1 ? 0 : t[v].cnt;
    }
};

// ---- 使用例 ----
// 関数内に貼る例。addだけではcntやendは増えない。
// 使用例
Trie<char> trie;
trie.insert(string("cat"));
trie.insert(string("car"));
cout << trie.count(string("cat")) << endl;
cout << trie.prefix_count(string("ca")) << endl; // など
// 列全体を insert するのではなく「既存の列に1個追加」なら、trie木のindexを第一引数に指定してadd() を直接使う
int to = trie.add(pos[a], x); // （返り値はトライ木のidx）

// ============================================================================
// 04. 区間をsetで管理 [l,r)
// ============================================================================
// ABC435E,ABC478Dなど

// ---- 区間の追加・削除・包含判定 ----
// 必要: ll, fi, se。sは互いに重ならない半開区間のset。
// 空のsetから開始し、このadd / eraseを通して更新する。隣接区間は併合しない。
struct {
    // 追加 / return: union 長の増加量
    ll add(set<pair<ll, ll>> &s, ll l, ll r) {
        if (l >= r)
            return 0;
        ll old = 0;
        auto it = s.lower_bound({l, LLONG_MIN});
        if (it != s.begin())
            it--;
        if (it != s.end() && it->se <= l)
            it++;

        while (it != s.end() && it->fi < r) {
            old += it->se - it->fi;
            l = min(l, it->fi);
            r = max(r, it->se);
            it = s.erase(it);
        }

        s.insert({l, r});
        return r - l - old;
    }

    // 削除 / return: union 長の減少量
    ll erase(set<pair<ll, ll>> &s, ll l, ll r) {
        if (l >= r)
            return 0;
        ll res = 0;
        auto it = s.lower_bound({l, LLONG_MIN});
        if (it != s.begin())
            it--;
        if (it != s.end() && it->se <= l)
            it++;

        while (it != s.end() && it->fi < r) {
            auto [a, b] = *it;
            res += min(r, b) - max(l, a);
            it = s.erase(it);

            if (a < l)
                s.insert({a, l});
            if (r < b) {
                s.insert({r, b});
                break;
            }
        }
        return res;
    }

    bool contains(const set<pair<ll, ll>> &s, ll x) {
        auto it = s.upper_bound({x, LLONG_MAX});
        if (it == s.begin())
            return false;
        it--;
        return x < it->se;
    }
} intervals;
