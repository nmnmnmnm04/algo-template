// 幾何：整数座標の直線・偏角ソート・角度判定
// このブロック全体をコピーして使う。C++17以降。
// 必要: #include <bits/stdc++.h> と using namespace std;（memo.cppの共通設定）。
// 直線の各操作 O(1)。入力座標の絶対値は 10^18 以下を想定。
// 判定は __int128 で正確に行い、交点・角度だけ long double に変換する。
// 浮動小数点座標の入力には対応しない。交点・角度には丸め誤差がある。

struct Point {
    long long x, y;
    long long id = -1; // 元の添字（不要なら省略可）
};

struct PointLD {
    long double x, y;
};

using i128 = __int128;
struct Vec128 {
    i128 x, y;
};
inline Vec128 difference(Point a, Point b) {
    return {i128(a.x) - b.x, i128(a.y) - b.y};
}
inline i128 cross(Vec128 a, Vec128 b) {
    return a.x * b.y - a.y * b.x;
}
inline i128 dot(Vec128 a, Vec128 b) {
    return a.x * b.x + a.y * b.y;
}

class Line {
    Point p_;
    Vec128 d_;

    Line(Point p, Vec128 d) : p_(p), d_(d) {}

  public:
    // a != b が前提。方向ベクトルの差を取る前に __int128 へ変換。
    static Line through(Point a, Point b) {
        assert(a.x != b.x || a.y != b.y);
        return Line(a, difference(b, a));
    }

    bool contains(Point p) const {
        return cross(d_, difference(p, p_)) == 0;
    }

    friend bool parallel(const Line &l, const Line &m);
    friend bool perpendicular(const Line &l, const Line &m);
    friend bool same_line(const Line &l, const Line &m);
    friend optional<PointLD> intersection_ld(const Line &l, const Line &m);
};

// 逆向き・同一直線も含めて平行なら true。
inline bool parallel(const Line &l, const Line &m) {
    return cross(l.d_, m.d_) == 0;
}

inline bool perpendicular(const Line &l, const Line &m) {
    return dot(l.d_, m.d_) == 0;
}

inline optional<PointLD> intersection_ld(const Line &l, const Line &m) {
    const i128 den = cross(l.d_, m.d_);
    if (den == 0)
        return nullopt; // 平行・一致では一意な交点がない。
    const i128 num = cross(difference(m.p_, l.p_), m.d_);
    const long double t = (long double)num / (long double)den;
    return PointLD{(long double)l.p_.x + t * (long double)l.d_.x,
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
Point a{0, 0}, b{2, 2};
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

// 戻り値は optional<PointLD>。値があることを確認してから p->x, p->y を使う。
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
    Point a, b, c, d;
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

// 非零ベクトル同士。内積 = i128(a.x) * b.x + i128(a.y) * b.y。
// 内積 > 0: 90°未満、== 0: 90°、< 0: 90°超（90°以上なら <= 0）。
// 外積 > 0: a→bが反時計回り、< 0: 時計回り、== 0: 平行。
inline bool same_angle(Point a, Point b) {
    Vec128 va{a.x, a.y}, vb{b.x, b.y};
    return cross(va, vb) == 0 && dot(va, vb) > 0;
}

// なす角 [0, π]（ラジアン）。
inline long double angle_between(Point a, Point b) {
    Vec128 va{a.x, a.y}, vb{b.x, b.y};
    i128 cr = cross(va, vb);
    if (cr < 0) cr = -cr;
    return atan2l((long double)cr, (long double)dot(va, vb));
}
// 度数法: angle_between(a, b) * 180 / acosl(-1.0L)
// ∠ABC: dot(difference(A,B), difference(C,B))の符号で判定。

// ============================================================================
// 偏角ソート
// ============================================================================

// x軸の正方向から反時計回り（0 <= 偏角 < 2π）。必要: Point, i128。
// 原点は先頭、同じ向きなら原点に近い順。O(n log n)。
inline bool angle_less(Point a, Point b) {
    int ha = (a.y < 0 || (a.y == 0 && a.x < 0)); // 前半0、後半1
    int hb = (b.y < 0 || (b.y == 0 && b.x < 0));
    if (ha != hb) return ha < hb;

    i128 cr = i128(a.x) * b.y - i128(a.y) * b.x;
    if (cr != 0) return cr > 0; // 同じ半周内では外積で比較

    return i128(a.x) * a.x + i128(a.y) * a.y
         < i128(b.x) * b.x + i128(b.y) * b.y;
}

// vector<Point> points(n);
// rep(i, n) {
//     cin >> points[i].x >> points[i].y;
//     points[i].id = i;
// }
// sort(points.begin(), points.end(), angle_less);
// ソート後もpoints[i].idで元の添字を参照できる。
