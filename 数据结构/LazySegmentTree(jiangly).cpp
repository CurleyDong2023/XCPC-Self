// contest url: https://codeforces.com/contest/1824/problem/D
// this code copy from jiangly
#include <bits/stdc++.h>

using i64 = long long;
template<class Info, class Tag>
struct LazySegmentTree {
    int n;
    std::vector<Info> info;
    std::vector<Tag> tag;
    LazySegmentTree() : n(0) {}
    LazySegmentTree(int n_, Info v_ = Info()) {
        init(n_, v_);
    }
    template<class T>
    LazySegmentTree(std::vector<T> init_) {
        init(init_);
    }
    void init(int n_, Info v_ = Info()) {
        init(std::vector(n_, v_));
    }
    template<class T>
    void init(std::vector<T> init_) {
        n = init_.size();
        info.assign(4 << std::__lg(n), Info());
        tag.assign(4 << std::__lg(n), Tag());
        std::function<void(int, int, int)> build = [&](int p, int l, int r) {
            if (r - l == 1) {
                info[p] = init_[l];
                return;
            }
            int m = (l + r) / 2;
            build(2 * p, l, m);
            build(2 * p + 1, m, r);
            pull(p);
        };
        build(1, 0, n);
    }
    void pull(int p) {
        info[p] = info[2 * p] + info[2 * p + 1];
    }
    void apply(int p, const Tag &v) {
        info[p].apply(v);
        tag[p].apply(v);
    }
    void push(int p) {
        apply(2 * p, tag[p]);
        apply(2 * p + 1, tag[p]);
        tag[p] = Tag();
    }
    void modify(int p, int l, int r, int x, const Info &v) {
        if (r - l == 1) {
            info[p] = v;
            return;
        }
        int m = (l + r) / 2;
        push(p);
        if (x < m) {
            modify(2 * p, l, m, x, v);
        } else {
            modify(2 * p + 1, m, r, x, v);
        }
        pull(p);
    }
    void modify(int p, const Info &v) {
        modify(1, 0, n, p, v);
    }
    Info rangeQuery(int p, int l, int r, int x, int y) {
        if (l >= y || r <= x) {
            return Info();
        }
        if (l >= x && r <= y) {
            return info[p];
        }
        int m = (l + r) / 2;
        push(p);
        return rangeQuery(2 * p, l, m, x, y) + rangeQuery(2 * p + 1, m, r, x, y);
    }
    Info rangeQuery(int l, int r) {
        return rangeQuery(1, 0, n, l, r);
    }
    void rangeApply(int p, int l, int r, int x, int y, const Tag &v) {
        if (l >= y || r <= x) {
            return;
        }
        if (l >= x && r <= y) {
            apply(p, v);
            return;
        }
        int m = (l + r) / 2;
        push(p);
        rangeApply(2 * p, l, m, x, y, v);
        rangeApply(2 * p + 1, m, r, x, y, v);
        pull(p);
    }
    void rangeApply(int l, int r, const Tag &v) {
        return rangeApply(1, 0, n, l, r, v);
    }
    template<class F>
    int findFirst(int p, int l, int r, int x, int y, F pred) {
        if (l >= y || r <= x || !pred(info[p])) {
            return -1;
        }
        if (r - l == 1) {
            return l;
        }
        int m = (l + r) / 2;
        push(p);
        int res = findFirst(2 * p, l, m, x, y, pred);
        if (res == -1) {
            res = findFirst(2 * p + 1, m, r, x, y, pred);
        }
        return res;
    }
    template<class F>
    int findFirst(int l, int r, F pred) {
        return findFirst(1, 0, n, l, r, pred);
    }
    template<class F>
    int findLast(int p, int l, int r, int x, int y, F pred) {
        if (l >= y || r <= x || !pred(info[p])) {
            return -1;
        }
        if (r - l == 1) {
            return l;
        }
        int m = (l + r) / 2;
        push(p);
        int res = findLast(2 * p + 1, m, r, x, y, pred);
        if (res == -1) {
            res = findLast(2 * p, l, m, x, y, pred);
        }
        return res;
    }
    template<class F>
    int findLast(int l, int r, F pred) {
        return findLast(1, 0, n, l, r, pred);
    }
};

struct Tag {
    i64 a = 0;
    i64 al = 0;
    int x = -1;
    void apply(Tag t) {
        if (x == -1) {
            a += t.a;
        } else {
            al += t.a * x;
        }
        al += t.al;
        if (t.x != -1) {
            x = t.x;
        }
    }
};

struct Info {
    int len = 0;
    i64 sum = 0;
    i64 hsum = 0;
    
    void apply(Tag t) {
        hsum += sum * t.a;
        hsum += len * t.al;
        if (t.x != -1) {
            sum = 1LL * t.x * len;
        }
    }
};

Info operator+(Info a, Info b) {
    Info c;
    c.len = a.len + b.len;
    c.sum = a.sum + b.sum;
    c.hsum = a.hsum + b.hsum;
    return c;
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    
    int n, q;
    std::cin >> n >> q;
    
    std::vector<int> lst(n, -1), prev(n);
    std::vector<int> a(n);
    for (int i = 0; i < n; i++) {
        std::cin >> a[i];
        a[i]--;
    }
    
    for (int i = 0; i < n; i++) {
        prev[i] = lst[a[i]];
        lst[a[i]] = i;
    }
    
    std::vector<i64> ans(q);
    std::vector<std::vector<std::tuple<int, int, int, int>>> qry(n);
    
    for (int i = 0; i < q; i++) {
        int l, r, x, y;
        std::cin >> l >> r >> x >> y;
        l--, x--;
        qry[x].emplace_back(l, r, i, 1);
        if (y < n) {
            qry[y].emplace_back(l, r, i, -1);
        }
    }
    
    LazySegmentTree<Info, Tag> seg(n, Info{1, 0, 0});
    std::set<int> s{-1};
    
    auto add = [&](int x) {
        auto it = s.insert(x).first;
        seg.rangeApply(*std::prev(it) + 1, x + 1, {0, 0, x + 1});
    };
    
    for (int i = 0; i < n; i++) {
        if (lst[i] != -1) {
            add(lst[i]);
        }
    }
    for (int i = n - 1; i >= 0; i--) {
        seg.rangeApply(0, n, {1, 0, -1});
        for (auto [l, r, k, t] : qry[i]) {
            ans[k] += t * seg.rangeQuery(l, r).hsum;
        }
        if (prev[i] != -1) {
            add(prev[i]);
        }
        seg.rangeApply(i, i + 1, {0, 0, 0});
    }
    
    for (int i = 0; i < q; i++) {
        std::cout << ans[i] << "\n";
    }
    
    return 0;
}
