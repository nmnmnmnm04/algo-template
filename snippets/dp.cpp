// 動的計画法：部分文字列DP・bitDP・TSP・桁DP
// 用途に合う見出しから必要なコードだけコピーして使うコードメモ。
// 各ブロックは独立した例。ファイル全体をまとめて実行するためのものではない。
// 共通の型・マクロは ../memo.cpp の「00. 共通設定」を参照。
// 目次: 01. 部分文字列DP / 02. bitDP / 03. TSP / 04. 桁DP

// ============================================================================
// 01. ABC452D 部分文字列に対するDP
// ============================================================================

// ---- 各開始位置の状態をまとめて更新 ----
// 必要: ll, rep, drep。tが空でない場合の例。
// tを部分列として含まない部分文字列の個数を数える。
int main() {
    string s, t;
    cin >> s >> t;
    ll n = s.size();
    ll m = t.size();
    ll ans = n * (n + 1) / 2;
    vector<ll> dp(m + 1);
    // j文字目まで含んでいる
    rep(i, n) {
        dp[0]++;
        drep(j, m + 1) {
            if (j == 0)
                break;
            if (s[i] == t[j - 1]) {
                dp[j] += dp[j - 1];
                dp[j - 1] = 0;
            }
        }
        ans -= dp[m];
    }
    cout << ans << endl;
}

// ============================================================================
// 02. bitDP
// ============================================================================

// ---- 集合に要素を1つ追加する遷移 ----
// 前提: 0 <= N < 31、2^N個の状態がメモリ・時間制限に収まること。
// 計算量 O(N 2^N)。costはこの例では要素ごとの固定値。問題に合わせて遷移を変える。
// 元メモの「各集合の合計値」の例を、minによる最小化DPの初期値に合わせて整理。
#include <bits/stdc++.h>
using namespace std;

int main() {
    int N;
    cin >> N;

    vector<int> cost(N);
    for (int i = 0; i < N; i++)
        cin >> cost[i];

    int MAX = 1 << N;

    // 例：未使用要素を1つ追加し、各集合を作る最小コストを更新
    const long long INF = 1LL << 61;
    vector<long long> dp(MAX, INF);
    dp[0] = 0;

    for (int bit = 0; bit < (1 << N); bit++) {
        if (dp[bit] == INF)
            continue;
        for (int i = 0; i < N; i++) {
            if (!(bit & (1 << i))) {
                int next = bit | (1 << i);
                dp[next] = min(dp[next], dp[bit] + cost[i]);
            }
        }
    }

    // dp[bit] が「その集合の値」
}

// ============================================================================
// 03. TSP　bitDP
// ============================================================================

// ---- 頂点0から出発して全頂点を訪問し、0に戻る ----
// 前提: 1 <= N < 31。計算量 O(N^2 2^N)、空間 O(N 2^N)。
// distは頂点間の移動コスト。INFには到達可能な経路和より十分大きい値を使う。
#include <bits/stdc++.h>
using namespace std;

const long long INF = 1e18;

int main() {
    int N;
    cin >> N;

    vector<vector<long long>> dist(N, vector<long long>(N));
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            cin >> dist[i][j];
        }
    }

    int MAX = 1 << N;

    // dp[bit][i] = bitを訪問して最後がiのときの最小コスト
    vector<vector<long long>> dp(MAX, vector<long long>(N, INF));

    dp[1][0] = 0; // スタートを0とする

    for (int bit = 0; bit < MAX; bit++) {
        for (int now = 0; now < N; now++) {
            if (!(bit & (1 << now)))
                continue;
            if (dp[bit][now] == INF)
                continue;

            for (int next = 0; next < N; next++) {
                if (bit & (1 << next))
                    continue;

                int nbit = bit | (1 << next);
                dp[nbit][next] = min(dp[nbit][next], dp[bit][now] + dist[now][next]);
            }
        }
    }

    // 全訪問して0に戻る
    long long ans = INF;
    for (int i = 0; i < N; i++) {
        ans = min(ans, dp[MAX - 1][i] + dist[i][0]);
    }

    cout << ans << endl;
}

// ============================================================================
// 04. 桁DP
// ============================================================================

// 例：1〜Nのうち桁和がDの倍数になる整数の個数。必要: mint, rep。
// Nは非負整数の10進文字列（空でない）、D >= 1。時間 O(|N| * D * 10)、空間 O(D)。
// dp[i][j][k]:
// i = started（0: 0埋め中、1: 開始済み）。
// j = smaller（0: 上限と一致、1: 未満）。
// k以降 = 問題依存の状態（この例では桁和 % D）。必要ならlなどを追加。
// include_zero = trueなら整数0も数える。
mint digit_dp(const string &N, int D, bool include_zero = false) {
    vector dp(2, vector(2, vector<mint>(D))); // 問題依存: 状態の次元・範囲
    dp[0][0][0] = 1; // 問題依存: 初期状態

    for (char c : N) { // 上位から1桁ずつ決める
        int bound = c - '0';
        vector ndp(2, vector(2, vector<mint>(D))); // 問題依存: dpと同じ次元・範囲
        rep(started, 2) { // 0埋め中／数字が始まった状態を列挙
            rep(smaller, 2) { // 上限と一致／すでに小さい状態を列挙
                int limit = smaller ? 9 : bound;
                rep(state, D) { // 問題依存: 状態を列挙（ここでは桁和の余り）
                    // if (dp[started][smaller][state] == 0) continue; // 個数0の状態は遷移を省略
                    rep(digit, limit + 1) { // 次に置く数字を試す
                        int next_smaller = smaller || (digit < bound);

                        if (!started && digit == 0) {
                            // 0埋めは状態を更新しない
                            ndp[0][next_smaller][state] += dp[0][smaller][state];
                            continue;
                        }

                        int next_state = (state + digit) % D; // 問題依存: 状態の更新
                        ndp[1][next_smaller][next_state] += dp[started][smaller][state];
                    }
                }
            }
        }
        dp.swap(ndp);
    }

    mint ans = 0;
    rep(i, 2) { // 上限と一致／未満の両方を集計
        int j = 0; // 問題依存: 採用する状態（桁和の余り0）
        ans += dp[1][i][j];
    }
    if (include_zero) ans += 1; // 問題依存: 整数0が条件を満たすか（この例では満たす）
    return ans;
}

// cout << digit_dp(N, D).val() << '\n';
// 区間[L, R]は F(R) - F(L-1)。L = 0なら引く項は0。

// ---- 桁DP（コメント省略版） ----
mint digit_dp(const string &N, int D, bool include_zero = false) {
    vector dp(2, vector(2, vector<mint>(D)));
    dp[0][0][0] = 1;

    for (char c : N) {
        int bound = c - '0';
        vector ndp(2, vector(2, vector<mint>(D)));
        rep(started, 2) {
            rep(smaller, 2) {
                int limit = smaller ? 9 : bound;
                rep(state, D) {
                    // if (dp[started][smaller][state] == 0) continue;
                    rep(digit, limit + 1) {
                        int next_smaller = smaller || (digit < bound);

                        if (!started && digit == 0) {
                            ndp[0][next_smaller][state] += dp[0][smaller][state];
                            continue;
                        }

                        int next_state = (state + digit) % D;
                        ndp[1][next_smaller][next_state] += dp[started][smaller][state];
                    }
                }
            }
        }
        dp.swap(ndp);
    }

    mint ans = 0;
    rep(i, 2) {
        int j = 0;
        ans += dp[1][i][j];
    }
    if (include_zero) ans += 1;
    return ans;
}
