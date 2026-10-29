// 幾何：整数座標の内積・外積・直線・偏角ソート・角度判定
// このブロック全体をコピーして使う。C++17以降。
// 必要: #include <bits/stdc++.h>, using namespace std;, ll（memo.cppの共通設定）。
// 直線の各操作 O(1)。座標の差・積・和などの中間計算までllに収まることが前提。
// 大きな座標を扱うなら using i128 = __int128; を追加し、pointのx, y、
// dot/crossの戻り値、den/num/crをi128に書き換える（idはllのままでよい）。
// 戻り値だけを広げても遅いので、差や積を計算する前に座標の型を広げる。
// 交点・角度だけ long double に変換する。
// 浮動小数点座標の入力には対応しない。交点・角度には丸め誤差がある。

struct point {
    ll x, y;
    ll id = -1; // 元の添字（不要なら省略可）
};

struct point_ld {
    long double x, y;
};

inline point difference(point a, point b) {
    return {a.x - b.x, a.y - b.y};
}

// pointを原点からの2次元ベクトルとして扱う。idは使わない。
// 内積: ax*bx + ay*by。
inline ll dot(point a, point b) {
    return a.x * b.x + a.y * b.y;
}
// 外積（2次元ではスカラー）: ax*by - ay*bx。
// 正: a→bが反時計回り、負: 時計回り、0: 平行（零ベクトルを含む）。
inline ll cross(point a, point b) {
    return a.x * b.y - a.y * b.x;
}

// point u{1, 2}, v{3, 4};
// ll d = dot(u, v);                         // 11
// ll c = cross(u, v);                       // -2
// 点A, B, Cに対し、ベクトルABとACの内積・外積:
// dot(difference(B, A), difference(C, A));
// cross(difference(B, A), difference(C, A));

class Line {
    point p_;
    point d_;

    Line(point p, point d) : p_(p), d_(d) {}

  public:
    // a != b が前提。
    static Line through(point a, point b) {
        assert(a.x != b.x || a.y != b.y);
        return Line(a, difference(b, a));
    }

    bool contains(point p) const {
        return cross(d_, difference(p, p_)) == 0;
    }

    friend bool parallel(const Line &l, const Line &m);
    friend bool perpendicular(const Line &l, const Line &m);
    friend bool same_line(const Line &l, const Line &m);
    friend optional<point_ld> intersection_ld(const Line &l, const Line &m);
};

// 逆向き・同一直線も含めて平行なら true。
inline bool parallel(const Line &l, const Line &m) {
    return cross(l.d_, m.d_) == 0;
}

inline bool perpendicular(const Line &l, const Line &m) {
    return dot(l.d_, m.d_) == 0;
}

inline optional<point_ld> intersection_ld(const Line &l, const Line &m) {
    const ll den = cross(l.d_, m.d_);
    if (den == 0)
        return nullopt; // 平行・一致では一意な交点がない。
    const ll num = cross(difference(m.p_, l.p_), m.d_);
    const long double t = (long double)num / (long double)den;
    return point_ld{(long double)l.p_.x + t * (long double)l.d_.x,
                   (long double)l.p_.y + t * (long double)l.d_.y};
}

inline bool same_line(const Line &l, const Line &m) {
    return parallel(l, m) && l.contains(m.p_);
}

enum class LineRelation { Intersect, Parallel, Coincident };

inline LineRelation relation(const Line &l, const Line &m) {
    if (!parallel(l, m))
        return LineRelation::Intersect;
    return same_line(l, m) ? LineRelation::Coincident : LineRelation::Parallel;
}

// ============================================================================
// 使用例1: 各操作の意味（直線は両方向に無限に伸びる。線分ではない）
// ============================================================================
/*
point a{0, 0}, b{2, 2};
auto l = Line::through(a, b);                  // y = x
auto m = Line::through({0, 2}, {2, 0});         // y = -x + 2
auto n = Line::through({0, 1}, {2, 3});         // y = x + 1
auto r = Line::through({4, 4}, {-2, -2});       // y = x（lと逆向きに指定）

bool on = l.contains({1, 1});                  // true: (1,1)はl上
bool outside = l.contains({3, 3});             // true: 指定した2点の外も含む
bool off = l.contains({1, 2});                 // false
bool par = parallel(l, n);                    // true: lとnは平行
bool reversed = parallel(l, r);               // true: 逆向き・同一直線も平行
bool perp = perpendicular(l, m);              // true: lとmは垂直
bool same = same_line(l, r);                  // true: lとrは同一直線
bool different = same_line(l, n);             // false: 平行でも別の直線

auto rel1 = relation(l, m);                   // LineRelation::Intersect（交点が1つ）
auto rel2 = relation(l, n);                   // LineRelation::Parallel（交点なし）
auto rel3 = relation(l, r);                   // LineRelation::Coincident（一致）

// 戻り値は optional<point_ld>。値があることを確認してから p->x, p->y を使う。
if (auto p = intersection_ld(l, m)) {
    cout << p->x << ' ' << p->y << '\n';  // 1 1
}
auto no_point = intersection_ld(l, n);         // nullopt: 平行
auto coincident = intersection_ld(l, r);       // nullopt: 一致（一意に決まらない）

// Line::through({1, 1}, {1, 1});              // NG: 同じ2点では直線を作れない
*/

// ============================================================================
// 使用例2: 入力された2直線の位置関係と交点を出力する
// ライブラリの後に、以下のmainをコメントから外して置く。
// 入力: ax ay bx by（直線AB）, cx cy dx dy（直線CD）。A != B, C != D が前提。
// 入力例:
// 0 0 2 2
// 0 2 2 0
// 出力例:
// Intersect
// 1.000000000000000 1.000000000000000
// ============================================================================
/*
int main() {
    point a, b, c, d;
    cin >> a.x >> a.y >> b.x >> b.y;
    cin >> c.x >> c.y >> d.x >> d.y;
    auto l = Line::through(a, b);
    auto m = Line::through(c, d);

    auto rel = relation(l, m);
    if (rel == LineRelation::Coincident) {
        cout << "Coincident\n";
    } else if (rel == LineRelation::Parallel) {
        cout << "Parallel\n";
    } else {
        cout << "Intersect\n";
        if (auto p = intersection_ld(l, m)) {
            cout << fixed << setprecision(15)
                 << p->x << ' ' << p->y << '\n';
        }
    }
}
*/

// ============================================================================
// ベクトルの角度判定
// ============================================================================

// 非零ベクトル同士。内積はdot(a, b)、外積はcross(a, b)。
// 内積 > 0: 90°未満、== 0: 90°、< 0: 90°超（90°以上なら <= 0）。
// 外積 > 0: a→bが反時計回り、< 0: 時計回り、== 0: 平行。
inline bool same_angle(point a, point b) {
    return cross(a, b) == 0 && dot(a, b) > 0;
}

// なす角 [0, π]（ラジアン）。
inline long double angle_between(point a, point b) {
    ll cr = cross(a, b);
    if (cr < 0) cr = -cr;
    return atan2l((long double)cr, (long double)dot(a, b));
}
// 度数法: angle_between(a, b) * 180 / acosl(-1.0L)
// ∠ABC: dot(difference(A,B), difference(C,B))の符号で判定。

// ============================================================================
// 偏角ソート
// ============================================================================

// x軸の正方向から反時計回り（0 <= 偏角 < 2π）。必要: point, ll。
// 原点は先頭、同じ向きなら原点に近い順。O(n log n)。
// 単独でコピーして使えるよう、内積・外積はこの関数内で直接計算する。
// 積・和がllに収まらないなら、pointのx, yとcrをi128に書き換える。
inline bool angle_less(point a, point b) {
    int ha = (a.y < 0 || (a.y == 0 && a.x < 0)); // 前半0、後半1
    int hb = (b.y < 0 || (b.y == 0 && b.x < 0));
    if (ha != hb) return ha < hb;

    ll cr = a.x * b.y - a.y * b.x;
    if (cr != 0) return cr > 0; // 同じ半周内では外積で比較

    return a.x * a.x + a.y * a.y
         < b.x * b.x + b.y * b.y;
}

// vector<point> points(n);
// rep(i, n) {
//     cin >> points[i].x >> points[i].y;
//     points[i].id = i;
// }
// sort(points.begin(), points.end(), angle_less);
// ソート後もpoints[i].idで元の添字を参照できる。
