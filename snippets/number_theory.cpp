// 数論：素数関連ライブラリ・組合せ
// 用途に合う見出しから必要なコードだけコピーして使うコードメモ。
// 各ブロックは独立した例。ファイル全体をまとめて実行するためのものではない。
// 共通の型・マクロは ../memo.cpp の「00. 共通設定」を参照。
// 目次: 01. 素数関連 / 02. 階乗でnCk / 03. modintでnCk

// ============================================================================
// 01. 素数関連ライブラリ(素数判定,素因数分解,約数の個数)
// ============================================================================

// ---- Miller-Rabin / Pollard's rho ----
// 必要: ll, u128。factorize / divisor_count の入力は n >= 1。
// 注意: 元実装のpollard_rho内の加算は、nがll上限付近だとオーバーフローし得る。
struct {
  private:
    // 内部処理用
    inline ll mod_mul(ll a, ll b, ll mod) {
        return (u128)a * b % mod;
    }

    inline ll mod_pow(ll a, ll e, ll mod) {
        ll r = 1;
        while (e > 0) {
            if (e & 1)
                r = mod_mul(r, a, mod);
            a = mod_mul(a, a, mod);
            e >>= 1;
        }
        return r;
    }

    inline ll pollard_rho(ll n) {
        if (n % 2 == 0)
            return 2;
        static mt19937_64 rng(chrono::steady_clock::now().time_since_epoch().count());

        while (true) {
            ll c = uniform_int_distribution<ll>(1, n - 1)(rng);
            ll x = uniform_int_distribution<ll>(0, n - 1)(rng);
            ll y = x;
            ll d = 1;

            auto f = [&](ll v) { return (mod_mul(v, v, n) + c) % n; };

            while (d == 1) {
                x = f(x);
                y = f(f(y));
                d = gcd(abs(x - y), n);
            }

            if (d != n)
                return d;
        }
    }

    inline void factorize_rec(ll n, map<ll, ll> &mp) {
        if (n == 1)
            return;
        if (is_prime(n)) {
            mp[n]++;
            return;
        }
        ll d = pollard_rho(n);
        factorize_rec(d, mp);
        factorize_rec(n / d, mp);
    }

  public:
    // 素数判定 (Miller-Rabin)
    inline bool is_prime(ll n) {
        if (n < 2)
            return false;
        for (ll p : {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37}) {
            if (n % p == 0)
                return n == p;
        }

        ll d = n - 1, s = 0;
        while ((d & 1) == 0) {
            d >>= 1;
            s++;
        }

        for (ll a : {2, 325, 9375, 28178, 450775, 9780504, 1795265022}) {
            if (a % n == 0)
                continue;
            ll x = mod_pow(a, d, n);
            if (x == 1 || x == n - 1)
                continue;

            bool comp = true;
            for (int r = 1; r < s; r++) {
                x = mod_mul(x, x, n);
                if (x == n - 1) {
                    comp = false;
                    break;
                }
            }
            if (comp)
                return false;
        }
        return true;
    }

    // 高速な素因数分解 (Pollard's rho)
    // 戻り値: {素因数, 指数} のベクター（素因数の昇順）
    inline vector<pair<ll, ll>> factorize(ll n) {
        map<ll, ll> mp;
        factorize_rec(n, mp);
        return {mp.begin(), mp.end()};
    }

    // 約数の個数を計算（素因数分解の結果から）
    inline ll divisor_count(const vector<pair<ll, ll>> &f) {
        ll res = 1;
        for (const auto &[p, e] : f) {
            res *= (e + 1);
        }
        return res;
    }

    // 約数の個数を計算（数字を直接渡す）
    inline ll divisor_count(ll n) {
        return divisor_count(factorize(n));
    }

} primes;

// ============================================================================
// 02. コンビネーションABC458Eなど
// ============================================================================

// ---- 階乗・逆階乗の前計算 ----
// 必要: ll。前提: modは素数、0 <= 最大値 < mod。
// 前計算 O(最大値 + log mod)、C(n,k)は O(1)。問い合わせnは前計算の範囲内。
struct comb { //conb C(最大値,mod)で宣言,C(n,k)で値を取得
    int MOD;
    vector<ll> fact, inv_fact;

    comb(int n, int mod) : MOD(mod), fact(n + 1), inv_fact(n + 1) {
        fact[0] = 1;

        for (int i = 1; i <= n; i++) {
            fact[i] = fact[i - 1] * i % MOD;
        }

        inv_fact[n] = modpow(fact[n], MOD - 2);

        for (int i = n; i >= 1; i--) {
            inv_fact[i - 1] = inv_fact[i] * i % MOD;
        }
    }

    ll modpow(ll a, ll e) const {
        ll r = 1;

        while (e > 0) {
            if (e & 1)
                r = r * a % MOD;
            a = a * a % MOD;
            e >>= 1;
        }

        return r;
    }

    ll operator()(int n, int k) const {
        if (k < 0 || k > n)
            return 0;

        return fact[n] * inv_fact[k] % MOD * inv_fact[n - k] % MOD;
    }
};

// ============================================================================
// 03. nCk等を求める愚直なcomb
// ============================================================================

// ---- modintで逐次計算 ----
// 必要: ll, mint。前提: 0 <= q <= p、min(q, p-q) < mint::mod()。
// 計算量 O(min(q, p-q) log mod)。
mint comb(ll p, ll q) {
    mint re = 1;
    q = max(q, p - q);
    for (ll i = p; i > q; i--) {
        re *= mint(i);
        re *= mint(p - i + 1).inv();
    }
    return re;
}
