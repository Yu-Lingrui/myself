class Solution {
public:
    vector<vector<int>> combinationSum(vector<int>& candidates, int target) {
        vector<vector<int>> res;
        vector<int> path;
        dfs(candidates, candidates.begin(), candidates.end(), path, res, target);
        return res;
    }

private:
    void dfs(vector<int>& candidates, vector<int>::iterator begin, vector<int>::iterator end, vector<int>& path, vector<vector<int>>& res, int target) {
        if (target < 0) {
            return;
        }
        if (target == 0) {
            res.push_back(path);
            return;
        }

        for (auto it = begin; it != end; ++it) {
            path.push_back(*it);
            dfs(candidates, it, end, path, res, target - *it);
            path.pop_back();
        }
    }
};
