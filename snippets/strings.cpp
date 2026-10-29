// 文字列：ローリングハッシュ
// 用途に合う見出しから必要なコードだけコピーして使うコードメモ。
// 各ブロックは独立した例。ファイル全体をまとめて実行するためのものではない。
// 共通の型・マクロは ../memo.cpp の「00. 共通設定」を参照。
// 短いランレングス圧縮や文字操作は ../memo.cpp を参照。

// ============================================================================
// 01. ロリハテンプレ
// ============================================================================

// ---- RollingHashと使用例 ----
// 必要: u64, u128。get(l,r)は半開区間 [l,r) のハッシュ。
// BASEはオブジェクトごとにランダムなので、別オブジェクト間の値をそのまま比較しない。
// ハッシュの一致には衝突の可能性がある。
struct RollingHash {
    static constexpr u64 MOD = (1ULL << 61) - 1;
    u64 BASE;
    vector<u64> h, p;

    // 行列の積などで使う 2^61-1 用の安全な乗算
    static u64 mul(u128 a, u128 b) {
        u128 t = a * b;
        t = (t >> 61) + (t & MOD);
        return (t >= MOD) ? t - MOD : t;
    }

    RollingHash(const string &s) {
        // BASE を実行時にランダムに決定（ハッシュ衝突対策）
        mt19937_64 mt(chrono::steady_clock::now().time_since_epoch().count());
        uniform_int_distribution<u64> dist(777, MOD - 777);
        BASE = dist(mt);

        int n = s.size();
        h.assign(n + 1, 0);
        p.assign(n + 1, 1);

        for (int i = 0; i < n; i++) {
            h[i + 1] = mul(h[i], BASE) + s[i];
            if (h[i + 1] >= MOD)
                h[i + 1] -= MOD;
            p[i + 1] = mul(p[i], BASE);
        }
    }

    // [l, r) のハッシュを取得
    u64 get(int l, int r) const {
        u64 res = h[r] + MOD - mul(h[l], p[r - l]);
        return (res >= MOD) ? res - MOD : res;
    }
};

int main() {
    string s;
    cin >> s;
    // 準備
    RollingHash rh(s);
    // 使用例: [l, r) のハッシュを取得
    u64 hash = rh.get(0, s.size());
}
