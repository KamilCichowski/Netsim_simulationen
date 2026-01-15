class Factory {
public:
    bool is_consistent() const;

    void do_deliveries(Time t);
    void do_package_passing();
    void do_work(Time t);

private:
    enum class NodeColor { UNVISITED, VISITED, VERIFIED };

    bool has_reachable_storehouse(
        const PackageSender* sender,
        std::map<const PackageSender*, NodeColor>& node_colors
    ) const;

};

