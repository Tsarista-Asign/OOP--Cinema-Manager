# ================== Linear Regression Pipeline (Full OOP) ==================
# Yêu cầu:
# - Tất cả cột trừ cột cuối là X (biến độc lập), cột cuối là y (biến phụ thuộc).
# - EDA: scatter Xi vs y + nhận xét chiều và độ mạnh (r).
# - Hồi quy đơn biến: stats.linregress => slope, intercept, r, p, std_err + vẽ đường hồi quy.
# - Hồi quy đa biến: statsmodels.OLS (kết quả chất lượng, có summary).
# - Diễn giải + Đánh giá: MAE, RMSE, R² (tự tính).
# - Dự đoán linh hoạt (np.ndarray / DataFrame), kiểm tra số cột.

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from scipy import stats
import statsmodels.api as sm
import statsmodels.stats.api as sms
from typing import Union, Tuple

class LinearRegressionPipeline:
    def __init__(self, df: pd.DataFrame):
        """
        df: DataFrame chỉ chứa dữ liệu số.
        - Các cột đầu: biến độc lập (X)
        - Cột cuối: biến phụ thuộc (y)
        """
        if not isinstance(df, pd.DataFrame):
            raise TypeError("df phải là pandas.DataFrame")
        if df.shape[1] < 2:
            raise ValueError("Cần ít nhất 2 cột: >=1 feature và 1 target")

        self.df = df.copy()
        self.feature_names = self.df.columns[:-1]
        self.target_name = self.df.columns[-1]

        # Kết quả đơn biến cho từng Xi
        self.results_univariate = {}  # {col: {"slope", "intercept", "r", "p", "std_err"}}

        # Kết quả đa biến (statsmodels)
        self.model_sm = None
        self.results_sm = None
        self.fitted_multi = False

    # ----------------------- Helpers -----------------------
    @staticmethod
    def _strength_label(abs_r: float) -> str:
        # ngưỡng tham khảo: 0-0.3: yếu, 0.3-0.5: vừa, 0.5-0.7: khá, >0.7: mạnh
        if abs_r < 0.3:
            return "yếu"
        elif abs_r < 0.5:
            return "vừa"
        elif abs_r < 0.7:
            return "khá"
        return "mạnh"

    def _coerce_X_new(
        self, X_new: Union[np.ndarray, pd.DataFrame, list]
    ) -> Tuple[np.ndarray, Union[pd.Index, None]]:
        """
        Chuẩn hóa X_new:
        - Nếu nhiều cột hơn -> lấy đúng số lượng cần.
        - Nếu ít cột hơn -> bổ sung bằng giá trị trung bình từ dữ liệu gốc.
        """
        expected = len(self.feature_names)

        if isinstance(X_new, pd.DataFrame):
            # ép về đúng thứ tự feature_names
            Xo = X_new.reindex(columns=self.feature_names)
            # nếu thiếu thì bổ sung bằng mean
            for col in self.feature_names:
                if col not in Xo or Xo[col].isna().any():
                    mean_val = self.df[col].mean()
                    Xo.loc[:, col] = Xo[col].fillna(mean_val)
            return Xo.to_numpy(), Xo.index

        # list hoặc numpy
        Xo = np.array(X_new, dtype=float)
        if Xo.ndim == 1:
            Xo = Xo.reshape(1, -1)

        n_features_in = Xo.shape[1]

        if n_features_in > expected:
            Xo = Xo[:, :expected]
        elif n_features_in < expected:
            # bổ sung bằng mean
            for i in range(n_features_in, expected):
                mean_val = self.df[self.feature_names[i]].mean()
                extra = np.full((Xo.shape[0], 1), mean_val)
                Xo = np.hstack([Xo, extra])

        return Xo, None

    # ----------------------- 1) EDA -----------------------
    def explore(self):
        """Vẽ scatter Xi vs y và nhận xét chiều + độ mạnh dựa trên r."""
        y = self.df[self.target_name].values
        for col in self.feature_names:
            x = self.df[col].values

            # Vẽ scatter
            plt.scatter(x, y, alpha=0.75)
            plt.xlabel(col)
            plt.ylabel(self.target_name)
            plt.title(f"Scatter: {col} vs {self.target_name}")
            plt.show()

            # Tính r và in nhận xét
            r = np.corrcoef(x, y)[0, 1]
            direction = "tăng (tương quan dương)" if r > 0 else "giảm (tương quan âm)"
            label = self._strength_label(abs(r))
            print(
                f"Mối quan hệ {col} → {self.target_name}: {direction}, "
                f"độ mạnh = {label}, r = {r:.3f}"
            )

    # ----------------------- 2) Univariate Regressions -----------------------
    def fit_univariate(self):
        """
        Hồi quy tuyến tính đơn biến cho từng Xi với y bằng stats.linregress,
        vẽ đường hồi quy trên scatter, lưu slope/intercept/r/p/std_err.
        """
        y = self.df[self.target_name].values

        for col in self.feature_names:
            x = self.df[col].values
            slope, intercept, r, p, std_err = stats.linregress(x, y)

            self.results_univariate[col] = {
                "slope": slope,
                "intercept": intercept,
                "r": r,
                "p": p,
                "std_err": std_err,
            }

            # Vẽ scatter + đường hồi quy mượt trên miền giá trị của x
            xs = np.linspace(x.min(), x.max(), 200)
            ys = slope * xs + intercept
            plt.scatter(x, y, color="blue", alpha=0.75, label="Dữ liệu")
            plt.plot(xs, ys, color="red", label="Đường hồi quy")
            plt.xlabel(col)
            plt.ylabel(self.target_name)
            plt.title(f"Hồi quy đơn biến: {col} → {self.target_name}")
            plt.legend()
            plt.show()

            print(f"\nKết quả hồi quy (đơn biến) cho {col}:")
            print(f"  Slope: {slope:.6f}")
            print(f"  Intercept: {intercept:.6f}")
            print(f"  r: {r:.6f}, R²: {r**2:.6f}")
            print(f"  p-value: {p:.6g}, StdErr: {std_err:.6f}")

    def interpret_univariate(self):
        """Diễn giải ý nghĩa slope/intercept/r/R²/p cho từng hồi quy đơn biến."""
        if not self.results_univariate:
            print("Chưa có kết quả đơn biến. Hãy gọi fit_univariate() trước.")
            return

        for col, res in self.results_univariate.items():
            print(f"\n=== Diễn giải {col} → {self.target_name} ===")
            print(
                f"- Slope ({res['slope']:.4f}): Mỗi 1 đơn vị tăng của {col}, "
                f"{self.target_name} tăng trung bình {res['slope']:.4f} (giữ ngữ cảnh đơn biến)."
            )
            print(
                f"- Intercept ({res['intercept']:.4f}): Khi {col} = 0, "
                f"{self.target_name} ước lượng là {res['intercept']:.4f}."
            )
            print(
                f"- r = {res['r']:.4f}, R² = {res['r']**2:.4f}: mức độ giải thích của mô hình đơn biến."
            )
            print(
                f"- p-value = {res['p']:.6g}: "
                + ("có ý nghĩa thống kê (slope khác 0)." if res['p'] < 0.05 else "không có ý nghĩa thống kê.")
            )

    # ----------------------- 3) Multivariate OLS (quality) -----------------------
    def fit_multivariate_ols(self, show_summary: bool = True):
        """
        Hồi quy đa biến bằng statsmodels.OLS (chất lượng cao, có p-value, CI, R², Adj R²).
        - Nếu phát hiện heteroscedasticity, tự động dùng robust SE (HC3).
        """
        X = self.df[self.feature_names]
        y = self.df[self.target_name]
        Xc = sm.add_constant(X)

        self.model_sm = sm.OLS(y, Xc)
        self.results_sm = self.model_sm.fit()
        self.fitted_multi = True

        # --- Kiểm tra heteroscedasticity ---
        _, pval, _, _ = sms.het_breuschpagan(self.results_sm.resid, self.results_sm.model.exog)

        if pval < 0.05:
            print("⚠️ Phát hiện heteroscedasticity → tự động dùng robust SE (HC3).")
            self.results_sm = self.model_sm.fit(cov_type="HC3")

        if show_summary:
            print(self.results_sm.summary())

        # In tóm tắt hệ số theo tên cột
        print("\n=== Hệ số (đa biến) ===")
        coef_series = self.results_sm.params
        for name, val in coef_series.items():
            if name == "const":
                print(f"Intercept: {val:.6f}")
            else:
                print(f"Hệ số {name}: {val:.6f}")

        return self.results_sm

    def predict_multivariate(self, X_new: Union[np.ndarray, pd.DataFrame, list]) -> Union[np.ndarray, pd.Series]:
        """
        Dự đoán bằng mô hình đa biến OLS đã fit.
        - Nhận np.ndarray / list / DataFrame.
        - Nếu DataFrame: sắp xếp đúng cột theo feature_names.
        - Nếu ít cột hơn: tự động bổ sung bằng giá trị trung bình.
        - Nếu nhiều cột hơn: chỉ lấy đủ số cột cần thiết.
        """
        if not self.fitted_multi or self.results_sm is None:
            raise Exception("Mô hình đa biến chưa huấn luyện. Gọi fit_multivariate_ols() trước.")

        X_arr, maybe_index = self._coerce_X_new(X_new)

        # tạo DataFrame với đúng số cột features
        X_df = pd.DataFrame(X_arr, columns=self.feature_names)

        # thêm constant (intercept)
        Xc = sm.add_constant(X_df, has_constant="add")

        # dự đoán
        yhat = self.results_sm.predict(Xc)

        if maybe_index is not None:
            return pd.Series(yhat.values, index=maybe_index, name=f"pred_{self.target_name}")
        return yhat.values

    def evaluate_multivariate(self) -> pd.DataFrame:
        """Tự tính MAE, RMSE, R² trên tập huấn luyện từ mô hình đa biến OLS."""
        if not self.fitted_multi or self.results_sm is None:
            raise Exception("Mô hình đa biến chưa huấn luyện. Gọi fit_multivariate_ols() trước.")

        X = self.df[self.feature_names]
        y = self.df[self.target_name].values
        y_pred = self.predict_multivariate(X)

        mae = np.mean(np.abs(y - y_pred))
        rmse = np.sqrt(np.mean((y - y_pred) ** 2))
        ss_res = np.sum((y - y_pred) ** 2)
        ss_tot = np.sum((y - np.mean(y)) ** 2)
        r2 = 1 - ss_res / ss_tot

        metrics = pd.DataFrame({"MAE": [mae], "RMSE": [rmse], "R²": [r2]})
        print("\n=== Đánh giá mô hình đa biến (train) ===")
        print(metrics)

        # Tham khảo thêm từ statsmodels (tuỳ chọn)
        try:
            print(f"R² (statsmodels): {self.results_sm.rsquared:.6f}")
            print(f"Adjusted R²: {self.results_sm.rsquared_adj:.6f}")
        except Exception:
            pass

        return metrics

    # (Tuỳ chọn) Vẽ residual plot để kiểm tra giả định tuyến tính/độ phân tán sai số
    def plot_residuals(self):
        if not self.fitted_multi or self.results_sm is None:
            print("Chưa có mô hình đa biến. Gọi fit_multivariate_ols() trước.")
            return
        X = self.df[self.feature_names]
        y = self.df[self.target_name].values
        y_pred = self.predict_multivariate(X)
        residuals = y - y_pred

        plt.scatter(y_pred, residuals, alpha=0.75)
        plt.axhline(0, linestyle="--")
        plt.xlabel("Giá trị dự đoán")
        plt.ylabel("Residuals (y - ŷ)")
        plt.title("Residual Plot (đa biến)")
        plt.show()

