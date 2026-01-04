# Libraries/PolynomialRegression.py
# -*- coding: utf-8 -*-

"""
AutoPolynomialRegression
========================

Quy trình tự động & có luận chứng trực quan cho bài toán hồi quy (đơn/đa biến) và đa thức:

Bước 1) Đọc dữ liệu truyền vào
    - Nhận DataFrame (các cột đầu là X, cột cuối là y), lọc numeric, drop NaN.

Bước 2) Đưa ra biểu đồ dữ liệu
    - Vẽ scatter từng Xi vs y (giới hạn tối đa để không rối).

Bước 3) Nhận định các biến
    - Mặc định cột cuối là biến phụ thuộc y; các cột còn lại là biến độc lập X.
    - Tính hệ số tương quan r, xác định chiều (dương/âm) và độ mạnh (kém/yếu/vừa/khá/mạnh).

Bước 4) Xác định số lượng biến để chọn đa hồi quy hay hồi quy đơn
    - Nếu số feature = 1 → hồi quy đơn biến; >1 → đa hồi quy.

Bước 5) Xác định bậc cao nhất để chọn đa thức hay tuyến tính
    - Duyệt degree = 1..max_degree; với mỗi degree:
        * Pipeline: PolynomialFeatures(degree, include_bias=False)
          → StandardScaler → LinearRegression
        * Tính Train RMSE, Test RMSE, Test R²
        * 5-fold CV RMSE (mean ± std)
    - In bảng minh chứng (deg | Train RMSE | Test RMSE | Test R2 | CV RMSE mean±std)
    - Chọn:
        * Best degree = degree có CV RMSE nhỏ nhất
        * 1-SE rule degree = degree nhỏ nhất có CV RMSE ≤ (min + std của best)
      Dùng chiến lược choose_by = "1se" (khuyến nghị) hoặc "best".

Bước 6) Thực hiện huấn luyện chính thức
    - Fit lại pipeline với degree đã chọn.

Bước 7) Đưa ra biểu đồ dự đoán
    - 1 feature: scatter dữ liệu + đường dự đoán mượt.
    - Nhiều feature: partial plots (thay đổi từng Xi, giữ biến khác ở mean).

Bước 8) Test & đánh giá
    - In bảng MAE, RMSE, R², Adjusted R² cho train & test.
    - Vẽ residual plot (test) + histogram residuals (test).

Lưu ý:
- Không dùng seaborn; chỉ matplotlib.
- Chuẩn hoá đặc trưng trước khi hồi quy để ổn định số học.
- Output ở từng bước đều rõ ràng, đủ để “chứng minh vì sao lựa chọn”.

(NEW)
- Thêm cấu hình cpu_cores, ram_gb, safety_ratio, min_feature, min_degree.
- Thêm bước tuỳ chọn select_features_and_degree() để chọn tập feature & degree tối đa phù hợp RAM.
"""

from typing import Optional, List, Tuple
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import math

from sklearn.preprocessing import PolynomialFeatures, StandardScaler
from sklearn.pipeline import Pipeline
from sklearn.linear_model import LinearRegression
from sklearn.model_selection import KFold, cross_val_score, train_test_split
from sklearn.metrics import mean_absolute_error, mean_squared_error, r2_score


class AutoPolynomialRegression:
    def __init__(
        self,
        df: pd.DataFrame,
        max_degree: int = 5,
        cv_splits: int = 5,
        test_size: float = 0.2,
        random_state: int = 42,
        choose_by: str = "1se",  # "1se" hoặc "best"
        max_scatter_plots: int = 9,
        max_features: Optional[int] = None,
        # ===== NEW (tuỳ chọn, mặc định không đổi hành vi cũ) =====
        cpu_cores: int = -1,             # n_jobs cho cross_val_score (mặc định -1 như cũ)
        ram_gb: float = 16.0,            # RAM máy (GB)
        safety_ratio: float = 0.5,       # chỉ dùng %RAM này
        min_feature: Optional[int] = None,  # số feature tối thiểu (None → không kích hoạt bước chọn)
        min_degree: Optional[int] = None,   # degree tối thiểu (None → giữ nguyên từ 1)
    ):
        if not isinstance(df, pd.DataFrame):
            raise TypeError("df phải là pandas.DataFrame")

        # Lọc numeric, bỏ NaN
        numeric_df = df.select_dtypes(include=[np.number]).copy()
        if numeric_df.shape[1] < 2:
            raise ValueError("Cần >= 2 cột numeric (>=1 feature và 1 target).")
        numeric_df = numeric_df.dropna(axis=0)

        self.df = numeric_df
        self.feature_names = self.df.columns[:-1]
        self.target_name = self.df.columns[-1]
        self.n_features_ = len(self.feature_names)

        self.max_degree = int(max_degree)
        self.cv_splits = int(cv_splits)
        self.test_size = float(test_size)
        self.random_state = int(random_state)
        self.choose_by = str(choose_by).lower().strip()
        if self.choose_by not in {"1se", "best"}:
            raise ValueError("choose_by phải là '1se' hoặc 'best'.")
        self.max_scatter_plots = int(max_scatter_plots)
        self.max_features = max_features  # giữ nguyên tham số gốc (hiện chưa dùng)

        # Nội bộ (giữ như cũ)
        self.X_train: Optional[np.ndarray] = None
        self.X_test: Optional[np.ndarray] = None
        self.y_train: Optional[np.ndarray] = None
        self.y_test: Optional[np.ndarray] = None

        self.cv_results_: Optional[pd.DataFrame] = None
        self.best_degree_: Optional[int] = None
        self.deg_1se_: Optional[int] = None
        self.chosen_degree_: Optional[int] = None

        self.best_model_: Optional[Pipeline] = None
        self.metrics_: Optional[pd.DataFrame] = None

        # ===== NEW: thông tin & lựa chọn thêm =====
        self.cpu_cores = int(cpu_cores)
        self.ram_gb = float(ram_gb)
        self.safety_ratio = float(safety_ratio)
        self.min_feature_cfg = min_feature
        self.min_degree_cfg = min_degree

        self.N_: Optional[int] = None   # số dòng (gán ở read_and_split)
        self.M_: Optional[int] = None   # số cột (gán ở read_and_split)

        self._feature_rank_df: Optional[pd.DataFrame] = None  # cache bảng describe_variables
        self._active_feat_idx: Optional[np.ndarray] = None    # indices feature đang dùng (nếu đã chọn)
        self._active_feat_names: Optional[List[str]] = None   # tên feature đang dùng
        self._degree_limit_by_ram: Optional[int] = None       # degree tối đa theo RAM (nếu đã chọn)
        self._ram_estimate_gb: Optional[float] = None         # RAM ước lượng khi chọn

    # ===== Helpers =====
    @staticmethod
    def _strength_label(abs_r: float) -> str:
        if abs_r < 0.05:
            return "kém"
        elif abs_r < 0.3:
            return "yếu"
        elif abs_r < 0.5:
            return "vừa"
        elif abs_r < 0.7:
            return "khá"
        return "mạnh"

    @staticmethod
    def _rmse(y_true, y_pred) -> float:
        return float(np.sqrt(mean_squared_error(y_true, y_pred)))

    @staticmethod
    def _adj_r2(r2: float, n: int, p: int) -> float:
        """Adjusted R² với p = số predictors (không tính intercept)."""
        if n <= p + 1:
            return np.nan
        return 1 - (1 - r2) * (n - 1) / (n - p - 1)

    def _get_active_X(self, X: np.ndarray) -> np.ndarray:
        """Trả về X theo tập feature đang hoạt động (nếu đã chọn); mặc định: toàn bộ cột."""
        if self._active_feat_idx is not None:
            return X[:, self._active_feat_idx]
        return X

    def _ensure_feature_ranking(self) -> pd.DataFrame:
        """Đảm bảo có bảng tương quan (không in ra, chỉ tính lặng lẽ nếu chưa có)."""
        if self._feature_rank_df is not None:
            return self._feature_rank_df.copy()
        y = self.df[self.target_name].values
        rows: List[dict] = []
        for col in self.feature_names:
            x = self.df[col].values
            r = np.corrcoef(x, y)[0, 1]
            rows.append({
                "feature": col,
                "r": r,
                "chiều": "tăng (dương)" if r >= 0 else "giảm (âm)",
                "độ mạnh": self._strength_label(abs(r))
            })
        out = pd.DataFrame(rows).sort_values(by="r", key=np.abs, ascending=False).reset_index(drop=True)
        self._feature_rank_df = out.copy()
        return out

    # ===== Bước 1: đọc & chia tập =====
    def read_and_split(self) -> None:
        """Chia train/test theo test_size, random_state."""
        self.N_, self.M_ = self.df.shape  # ===== NEW: lưu N, M (không in) =====
        X = self.df[self.feature_names].values
        y = self.df[self.target_name].values
        self.X_train, self.X_test, self.y_train, self.y_test = train_test_split(
            X, y, test_size=self.test_size, random_state=self.random_state
        )

    # ===== Bước 2: biểu đồ dữ liệu =====
    def plot_data(self) -> None:
        """Scatter từng Xi vs y (tối đa self.max_scatter_plots)."""
        m = min(self.n_features_, self.max_scatter_plots)
        ncols = 3 if m >= 3 else m
        ncols = max(ncols, 1)
        nrows = int(np.ceil(m / ncols))

        fig, axes = plt.subplots(nrows, ncols, figsize=(4 * ncols, 3.2 * max(nrows, 1)))
        axes = np.atleast_1d(axes).reshape(-1)

        for i in range(m):
            col = self.feature_names[i]
            x = self.df[col].values
            y = self.df[self.target_name].values
            axes[i].scatter(x, y, s=12, alpha=0.8)
            axes[i].set_xlabel(col)
            axes[i].set_ylabel(self.target_name)
            axes[i].set_title(f"{col} vs {self.target_name}")

        for j in range(m, len(axes)):
            axes[j].axis("off")

        fig.suptitle("Scatter Xi vs y (phân bố dữ liệu)", y=1.02)
        fig.tight_layout()
        plt.show()

    # ===== Bước 3: nhận định biến =====
    def describe_variables(self) -> pd.DataFrame:
        """
        Bảng tương quan X_i với y (r, chiều, độ mạnh). Trả về DataFrame & in console.
        """
        y = self.df[self.target_name].values
        rows: List[dict] = []
        for col in self.feature_names:
            x = self.df[col].values
            r = np.corrcoef(x, y)[0, 1]
            rows.append({
                "feature": col,
                "r": r,
                "chiều": "tăng (dương)" if r >= 0 else "giảm (âm)",
                "độ mạnh": self._strength_label(abs(r))
            })
        out = pd.DataFrame(rows).sort_values(by="r", key=np.abs, ascending=False).reset_index(drop=True)

        # ===== NEW: cache để bước chọn không đổi format in ra ở đây =====
        self._feature_rank_df = out.copy()

        print("\n=== Nhận định biến (tương quan với y) ===")
        # print(f"{'feature':<20} | {'r':>7} | {'chiều':<12} | {'độ mạnh':<6}")
        # print("-" * 55)
        # for _, r in out.iterrows():
        #     print(f"{r['feature']:<20} | {r['r']:>+7.3f} | {r['chiều']:<12} | {r['độ mạnh']:<6}")
        return out

    # ===== NEW: Bước chọn feature & degree theo RAM (tuỳ chọn) =====
    def select_features_and_degree(
        self,
        min_degree: Optional[int] = None,
        safety_ratio: Optional[float] = None,
    ):
        """
        (NEW) Chọn tập feature & degree tối đa thỏa RAM_budget = ram_gb * safety_ratio.
        - min_feature = số feature được phân loại "mạnh" (|r| >= 0.7 theo _strength_label).
        - Đồng thời loại bỏ hoàn toàn các feature có |r| < 0.1.
        """

        min_degree_eff = (min_degree if min_degree is not None
                        else (self.min_degree_cfg if self.min_degree_cfg is not None else 1))
        safety_ratio_eff = safety_ratio if safety_ratio is not None else self.safety_ratio
        ram_budget_bytes = int(self.ram_gb * safety_ratio_eff * (1024 ** 3))

        # Lấy bảng xếp hạng từ describe_variables (hoặc tự tính nếu chưa gọi)
        rank_df = self._ensure_feature_ranking()

        # Loại bỏ feature rất yếu (|r| < 0.05)
        rank_df = rank_df[rank_df["r"].abs() >= 0.05].reset_index(drop=True)

        if len(rank_df) == 0:
            raise ValueError("Không có feature nào có |r| >= 0.05 để chọn.")

        # min_feature = số feature "mạnh" (|r| >= 0.7)
        strong_feats = rank_df[rank_df["độ mạnh"] == "mạnh"]["feature"].tolist()
        min_feature_eff = len(strong_feats)
        if min_feature_eff == 0:
            # fallback an toàn: ít nhất 1 feature
            min_feature_eff = 1

        all_feats = rank_df["feature"].tolist()

        if self.N_ is None or self.M_ is None:
            self.N_, self.M_ = self.df.shape

        best = None
        for f_count in range(min_feature_eff, len(all_feats) + 1):
            feats = all_feats[:f_count]
            d = max(1, min_degree_eff)
            while True:
                P = math.comb(f_count + d, d) - 1
                ram_bytes = 8 * self.N_ * P
                if ram_bytes > ram_budget_bytes:
                    break
                best = (feats.copy(), d, ram_bytes / (1024 ** 3))
                d += 1

        if best is None:
            raise MemoryError("Không tìm được cấu hình (feature, degree) phù hợp với RAM.")

        feats_chosen, d_max_by_ram, ram_est_gb = best
        idx = np.array([list(self.feature_names).index(c) for c in feats_chosen], dtype=int)

        self._active_feat_idx = idx
        self._active_feat_names = feats_chosen
        self._degree_limit_by_ram = int(d_max_by_ram)
        self._ram_estimate_gb = float(ram_est_gb)

        print("\n=== Kết quả chọn Feature & Degree theo RAM (NEW) ===")
        print(f"Số dòng N = {self.N_}, Số cột M = {self.M_}, Tổng feature gốc = {self.n_features_}")
        print(f"Số feature đủ điều kiện (|r| >= 0.1): {len(all_feats)}")
        print(f"Số feature mạnh (|r| >= 0.7): {min_feature_eff}")
        print(f"Feature được chọn: {feats_chosen}")
        print(f"Degree tối đa theo RAM: {d_max_by_ram}")
        print(f"Ước lượng RAM cho X_poly: ~{ram_est_gb:.2f} GB")

        return feats_chosen, d_max_by_ram, ram_est_gb


    # ===== Bước 4–6: chọn loại & degree, huấn luyện =====
    def select_degree_and_train(self) -> None:
        """
        - Xác định hồi quy đơn/đa biến theo số feature.
        - Duyệt degree 1..max_degree (hoặc tới _degree_limit_by_ram nếu đã gọi bước NEW),
          tính Train/Test/CV, in bảng minh chứng (format giữ nguyên).
        - Chọn degree theo 'best' hoặc '1se' và fit chính thức.
        """
        if self.X_train is None:
            self.read_and_split()

        # Chọn X sử dụng (nếu đã chọn feature theo RAM)
        X_tr_used = self._get_active_X(self.X_train)
        X_te_used = self._get_active_X(self.X_test)
        n_feats_used = X_tr_used.shape[1]

        kind = "Hồi quy đơn biến" if n_feats_used == 1 else "Đa hồi quy"
        print(f"\n>>> Loại mô hình theo số biến: {kind} (n_features = {n_feats_used})")

        # Dải degree: mặc định 1..max_degree; nếu đã có giới hạn theo RAM → thu hẹp
        deg_start = 1
        deg_end = self.max_degree
        if self._degree_limit_by_ram is not None:
            deg_end = min(deg_end, int(self._degree_limit_by_ram))
        degrees = list(range(deg_start, deg_end + 1))

        rmse_tr, rmse_te, r2_te = [], [], []
        cv_mean, cv_std = [], []
        kf = KFold(n_splits=self.cv_splits, shuffle=True, random_state=self.random_state)

        for d in degrees:
            pipe = Pipeline([
                ("poly", PolynomialFeatures(degree=d, include_bias=False)),
                ("scaler", StandardScaler()),
                ("linreg", LinearRegression())
            ])
            pipe.fit(X_tr_used, self.y_train)

            # Train/Test metrics
            yhat_tr = pipe.predict(X_tr_used)
            yhat_te = pipe.predict(X_te_used)
            rmse_tr.append(self._rmse(self.y_train, yhat_tr))
            rmse_te.append(self._rmse(self.y_test, yhat_te))
            r2_te.append(r2_score(self.y_test, yhat_te))

            # CV trên tập train
            scores = cross_val_score(
                pipe, X_tr_used, self.y_train,
                cv=kf, scoring="neg_mean_squared_error", n_jobs=self.cpu_cores  # ===== NEW: cpu_cores =====
            )
            rmse_cv = np.sqrt(-scores)
            cv_mean.append(float(rmse_cv.mean()))
            cv_std.append(float(rmse_cv.std()))

        # Bảng kết quả choices (giữ format cũ)
        self.cv_results_ = pd.DataFrame({
            "degree": degrees,
            "RMSE_train": rmse_tr,
            "RMSE_test": rmse_te,
            "R2_test": r2_te,
            "CV_RMSE_mean": cv_mean,
            "CV_RMSE_std": cv_std
        })

        # Chọn best & 1-SE (giữ logic cũ)
        best_idx = int(np.argmin(cv_mean))
        self.best_degree_ = degrees[best_idx]
        one_se_threshold = cv_mean[best_idx] + cv_std[best_idx]
        self.deg_1se_ = next(d for d, m in zip(degrees, cv_mean) if m <= one_se_threshold)
        self.chosen_degree_ = self.deg_1se_ if self.choose_by == "1se" else self.best_degree_

        # In bảng minh chứng (format cũ)
        print("\n=== Kết quả đánh giá các bậc đa thức ===")
        header = f"{'deg':>3} | {'Train RMSE':>10} | {'Test RMSE':>9} | {'Test R2':>7} | {'CV RMSE (mean±std)':>22}"
        print(header)
        print("-" * len(header))
        for _, row in self.cv_results_.iterrows():
            print(f"{int(row['degree']):>3} | "
                  f"{row['RMSE_train']:.4f} | "
                  f"{row['RMSE_test']:.4f} | "
                  f"{row['R2_test']:.4f} | "
                  f"{row['CV_RMSE_mean']:.4f} ± {row['CV_RMSE_std']:.4f}")

        print(f"\nBest degree (min CV RMSE): {self.best_degree_}")
        print(f"1-SE rule degree (đơn giản hơn, gần tối ưu): {self.deg_1se_}")
        strategy = "1-SE rule" if self.choose_by == "1se" else "Best CV"
        print(f"=> Degree được chọn theo chiến lược [{strategy}]: {self.chosen_degree_}")

        # Fit chính thức với degree đã chọn (giữ format/logic cũ)
        self.best_model_ = Pipeline([
            ("poly", PolynomialFeatures(degree=self.chosen_degree_, include_bias=False)),
            ("scaler", StandardScaler()),
            ("linreg", LinearRegression())
        ]).fit(X_tr_used, self.y_train)

    # ===== Trực quan hoá lựa chọn degree =====
    def plot_cv_results(self) -> None:
        """Biểu đồ RMSE theo degree (train/test/CV) + đánh dấu best/1-SE/chosen."""
        if self.cv_results_ is None:
            print("Chưa có kết quả CV. Hãy gọi select_degree_and_train().")
            return
        df = self.cv_results_
        plt.figure()
        plt.plot(df["degree"], df["RMSE_train"], marker="o", label="Train RMSE")
        plt.plot(df["degree"], df["RMSE_test"], marker="o", label="Test RMSE")
        plt.errorbar(df["degree"], df["CV_RMSE_mean"], yerr=df["CV_RMSE_std"],
                     marker="o", linestyle="-", label=f"CV RMSE ({self.cv_splits}-fold)")
        if self.best_degree_ is not None:
            plt.axvline(self.best_degree_, linestyle="--", label=f"Best degree = {self.best_degree_}")
        if self.deg_1se_ is not None:
            plt.axvline(self.deg_1se_, linestyle=":", label=f"1-SE degree = {self.deg_1se_}")
        if self.chosen_degree_ is not None:
            plt.axvline(self.chosen_degree_, linestyle="-.", label=f"Chosen = {self.chosen_degree_}")
        plt.xlabel("Polynomial degree")
        plt.ylabel("RMSE")
        plt.title("RMSE vs degree — minh chứng chọn bậc")
        plt.legend()
        plt.show()

    # ===== Bước 7: biểu đồ dự đoán =====
    def plot_predictions(self, n_points: int = 300) -> None:
        """
        - Nếu 1 feature: scatter (train/test) + đường dự đoán mượt.
        - Nếu nhiều feature: partial plots cho từng Xi (thay đổi Xi, giữ biến khác ở mean).
        """
        if self.best_model_ is None:
            print("Chưa có mô hình. Hãy gọi select_degree_and_train().")
            return

        # Dữ liệu/feature đang hoạt động
        X_tr_used = self._get_active_X(self.X_train)
        X_te_used = self._get_active_X(self.X_test)
        n_feats_used = X_tr_used.shape[1]

        if n_feats_used == 1:
            # Scatter train/test (giữ format cũ)
            plt.scatter(X_tr_used, self.y_train, s=18, alpha=0.8, label="Train")
            plt.scatter(X_te_used, self.y_test, s=18, alpha=0.8, color="red", label="Test")

            # Đường dự đoán
            x_min = float(np.min(X_tr_used))
            x_max = float(np.max(X_tr_used))
            xx = np.linspace(x_min, x_max, n_points).reshape(-1, 1)
            yy = self.best_model_.predict(xx)

            # Tên cột: nếu đã chọn → tên đó; else dùng cột đầu tiên
            if self._active_feat_names:
                xname = self._active_feat_names[0]
            else:
                xname = self.feature_names[0]

            plt.plot(xx, yy, linewidth=2, label=f"Fit (deg={self.chosen_degree_})")
            plt.xlabel(xname)
            plt.ylabel(self.target_name)
            plt.title("Scatter dữ liệu & đường dự đoán")
            plt.legend()
            plt.show()

        else:
            # Partial plots: thay đổi từng Xi, giữ biến khác ở mean (chỉ với feature đang dùng)
            if self._active_feat_names:
                feat_names = self._active_feat_names
            else:
                feat_names = list(self.feature_names)

            means = self.df[feat_names].mean().values
            n = len(feat_names)
            ncols = 3 if n >= 3 else n
            ncols = max(ncols, 1)
            nrows = int(np.ceil(n / ncols))
            fig, axes = plt.subplots(nrows, ncols, figsize=(4 * ncols, 3.2 * max(nrows, 1)))
            axes = np.atleast_1d(axes).reshape(-1)

            for i, col in enumerate(feat_names):
                ax = axes[i]
                x_col = self.df[col].values
                x_min, x_max = float(x_col.min()), float(x_col.max())
                grid = np.linspace(x_min, x_max, n_points)

                X_plot = np.tile(means, (n_points, 1))
                X_plot[:, i] = grid
                y_pred = self.best_model_.predict(X_plot)

                # scatter train
                ax.scatter(X_tr_used[:, i], self.y_train, s=10, alpha=0.7, label="Train")
                # scatter test (màu đỏ)
                ax.scatter(X_te_used[:, i], self.y_test, s=10, alpha=0.7, color="red", label="Test")

                # đường dự đoán (giữ nguyên)
                ax.plot(grid, y_pred, label=f"partial fit (deg={self.chosen_degree_})")

                ax.set_xlabel(col)
                ax.set_ylabel(self.target_name)
                ax.set_title(f"Ảnh hưởng riêng của {col}")
                ax.legend()

            for j in range(n, len(axes)):
                axes[j].axis("off")
            fig.suptitle("Partial plots — giữ biến khác ở mean", y=1.02)
            fig.tight_layout()
            plt.show()

    # Alias (phù hợp với cách gọi cũ)
    def plot_fit(self, n_points: int = 300) -> None:
        self.plot_predictions(n_points=n_points)

    # ===== NEW: Báo cáo tổng quan dữ liệu =====
    def report_overview(self) -> None:
        """
        Báo cáo tổng quan dữ liệu: kích thước, info, mô tả thống kê, missing values.
        """
        print("\n=== Tổng quan dữ liệu ===")
        print(f"Số dòng: {self.df.shape[0]}, Số cột: {self.df.shape[1]}")
        print("\n--- Info ---")
        print(self.df.info())
        print("\n--- Thống kê mô tả ---")
        print(self.df.describe().T.round(4))
        print("\n--- Missing values per column ---")
        print(self.df.isnull().sum())

    # ===== Bước 8 (cập nhật): đánh giá & test =====
    def evaluate(self) -> pd.DataFrame:
        """
        Trả về bảng MAE, RMSE, R², Adjusted R², r cho train & test.
        """
        if self.best_model_ is None:
            raise Exception("Chưa có mô hình. Gọi select_degree_and_train().")

        # Dữ liệu đang dùng (sau khi chọn feature)
        X_tr_used = self._get_active_X(self.X_train)
        X_te_used = self._get_active_X(self.X_test)

        # Train
        yhat_tr = self.best_model_.predict(X_tr_used)
        mae_tr = mean_absolute_error(self.y_train, yhat_tr)
        rmse_tr = self._rmse(self.y_train, yhat_tr)
        r2_tr = r2_score(self.y_train, yhat_tr)

        from scipy.stats import pearsonr
        r_tr, _ = pearsonr(self.y_train, yhat_tr)

        poly: PolynomialFeatures = self.best_model_.named_steps["poly"]
        p_no_bias = getattr(poly, "n_output_features_", None)
        if p_no_bias is None:
            p_no_bias = poly.fit_transform(X_tr_used[:1]).shape[1]

        adj_r2_tr = self._adj_r2(r2_tr, n=len(self.y_train), p=p_no_bias)

        # Test
        yhat_te = self.best_model_.predict(X_te_used)
        mae_te = mean_absolute_error(self.y_test, yhat_te)
        rmse_te = self._rmse(self.y_test, yhat_te)
        r2_te = r2_score(self.y_test, yhat_te)
        r_te, _ = pearsonr(self.y_test, yhat_te)
        adj_r2_te = self._adj_r2(r2_te, n=len(self.y_test), p=p_no_bias)

        report = pd.DataFrame([{
            "degree_chosen": self.chosen_degree_,
            "n_features_raw": self.n_features_,
            "n_features_poly": p_no_bias,
            "Train_MAE": mae_tr, "Train_RMSE": rmse_tr, "Train_R2": r2_tr,
            "Train_AdjR2": adj_r2_tr, "Train_r": r_tr,
            "Test_MAE": mae_te, "Test_RMSE": rmse_te, "Test_R2": r2_te,
            "Test_AdjR2": adj_r2_te, "Test_r": r_te
        }])

        print("\n=== Đánh giá mô hình cuối (train/test) ===")
        # print(report.round(4))
        self.metrics_ = report
        return report


    # ===== NEW: Nhận xét kết quả =====
    def interpret_results(self) -> None:
        """
        Đưa ra nhận xét cơ bản dựa trên kết quả evaluate().
        """
        if self.metrics_ is None:
            print("Chưa có kết quả evaluate(). Hãy gọi evaluate() trước.")
            return

        row = self.metrics_.iloc[0]
        print("\n=== Nhận xét kết quả ===")
        # R2
        if row["Test_R2"] > 0.8:
            print(f"- Mô hình giải thích tốt ({row['Test_R2']:.2f} R² trên test).")
        elif row["Test_R2"] > 0.5:
            print(f"- Mô hình giải thích ở mức trung bình ({row['Test_R2']:.2f} R² trên test).")
        else:
            print(f"- Mô hình giải thích kém ({row['Test_R2']:.2f} R² trên test).")

        # r
        print(f"- Hệ số tương quan Pearson r trên test = {row['Test_r']:.2f}.")

        # Sai số
        print(f"- MAE (test) = {row['Test_MAE']:.2f}, RMSE (test) = {row['Test_RMSE']:.2f}.")

        if row["Train_R2"] - row["Test_R2"] > 0.1:
            print("- Có dấu hiệu overfitting (Train R² cao hơn Test R² nhiều).")
        else:
            print("- Không có dấu hiệu overfitting rõ rệt.")

    def plot_residuals(self) -> None:
        """Residual plot & histogram (test) để kiểm tra giả định sai số."""
        if self.best_model_ is None:
            print("Chưa có mô hình. Hãy gọi select_degree_and_train().")
            return

        X_te_used = self._get_active_X(self.X_test)
        yhat_te = self.best_model_.predict(X_te_used)
        residuals = self.y_test - yhat_te

        fig, axes = plt.subplots(1, 2, figsize=(10, 3.5))

        axes[0].scatter(yhat_te, residuals, s=12, alpha=0.8)
        axes[0].axhline(0, linestyle="--")
        axes[0].set_xlabel("ŷ (test)")
        axes[0].set_ylabel("Residuals")
        axes[0].set_title("Residuals vs ŷ (test)")

        axes[1].hist(residuals, bins=20, alpha=0.9)
        axes[1].set_title("Histogram residuals (test)")
        axes[1].set_xlabel("Residual")
        axes[1].set_ylabel("Count")

        fig.tight_layout()
        plt.show()

    # ===== Tiện ích bổ sung =====
    def get_cv_table(self) -> Optional[pd.DataFrame]:
        """Trả về bảng CV theo degree."""
        return self.cv_results_.copy() if self.cv_results_ is not None else None

    def get_metrics_table(self) -> Optional[pd.DataFrame]:
        """Trả về bảng đánh giá cuối."""
        return self.metrics_.copy() if self.metrics_ is not None else None

    def run_full_pipeline(self) -> pd.DataFrame:
        """
        Chạy full quy trình với output đầy đủ (giữ nguyên thứ tự/in ấn cũ):
        - plot_data, describe_variables
        - select_degree_and_train (+ bảng & plot CV)
        - plot_predictions (đường fit)
        - evaluate (+ residual plots)
        Lưu ý: Bước chọn feature/degree theo RAM là TUỲ CHỌN, không tự động gọi ở đây
               để đảm bảo hành vi/format cũ không bị thay đổi.
        """
        self.read_and_split()
        self.plot_data()
        self.describe_variables()
        self.select_degree_and_train()
        self.plot_cv_results()
        self.plot_predictions()
        out = self.evaluate()
        self.plot_residuals()
        return out
