
import numpy as np, pandas as pd, matplotlib.pyplot as plt, torch
from torch import nn
from sklearn.datasets import load_breast_cancer
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import StandardScaler
from sklearn.metrics import (accuracy_score, precision_score, recall_score, f1_score,
                             roc_auc_score, roc_curve, confusion_matrix)
from sklearn.linear_model import LogisticRegression

SEED = 42
np.random.seed(SEED)
torch.manual_seed(SEED)

ds = load_breast_cancer()
X = ds.data.astype(np.float32)
y = ds.target.astype(np.int64)

X_train, X_temp, y_train, y_temp = train_test_split(X, y, test_size=0.4, random_state=SEED, stratify=y)
X_val, X_test, y_val, y_test = train_test_split(X_temp, y_temp, test_size=0.5, random_state=SEED, stratify=y_temp)

scaler = StandardScaler()
X_train_std = scaler.fit_transform(X_train)
X_val_std   = scaler.transform(X_val)
X_test_std  = scaler.transform(X_test)

Xtr = torch.tensor(X_train_std, dtype=torch.float32)
ytr = torch.tensor(y_train, dtype=torch.float32).view(-1,1)
Xva = torch.tensor(X_val_std, dtype=torch.float32)
yva = torch.tensor(y_val, dtype=torch.float32).view(-1,1)
Xte = torch.tensor(X_test_std, dtype=torch.float32)
yte = torch.tensor(y_test, dtype=torch.float32).view(-1,1)

class SLP(nn.Module):
    def __init__(self, d): super().__init__(); self.linear = nn.Linear(d,1)
    def forward(self, x): return self.linear(x)

def train_slp_fullbatch(Xtr, ytr, Xva, yva, lr_grid=(1e-1,1e-2), epochs=30):
    best_auc, best_state, best_lr = -1.0, None, None
    for lr in lr_grid:
        m = SLP(Xtr.shape[1])
        opt = torch.optim.SGD(m.parameters(), lr=lr)
        loss_fn = nn.BCEWithLogitsLoss()
        for _ in range(epochs):
            m.train(); opt.zero_grad()
            logits = m(Xtr); loss = loss_fn(logits, ytr); loss.backward(); opt.step()
        m.eval()
        with torch.no_grad():
            val_probs = torch.sigmoid(m(Xva)).numpy().ravel()
        auc = roc_auc_score(yva.numpy().ravel(), val_probs)
        if auc > best_auc:
            best_auc, best_state, best_lr = auc, {k:v.detach().clone() for k,v in m.state_dict().items()}, lr
    final = SLP(Xtr.shape[1]); final.load_state_dict(best_state); final.eval()
    return final, {"best_val_auc": float(best_auc), "best_lr": float(best_lr)}

def eval_metrics(y_true, y_prob, thr=0.5):
    y_pred = (y_prob>=thr).astype(int)
    return dict(
        accuracy=float(accuracy_score(y_true, y_pred)),
        precision=float(precision_score(y_true, y_pred, zero_division=0)),
        recall=float(recall_score(y_true, y_pred, zero_division=0)),
        f1=float(f1_score(y_true, y_pred, zero_division=0)),
        roc_auc=float(roc_auc_score(y_true, y_prob)),
    )

slp, info = train_slp_fullbatch(Xtr, ytr, Xva, yva, lr_grid=(1e-1,1e-2), epochs=30)
with torch.no_grad():
    prob_tr = torch.sigmoid(slp(Xtr)).numpy().ravel()
    prob_va = torch.sigmoid(slp(Xva)).numpy().ravel()
    prob_te = torch.sigmoid(slp(Xte)).numpy().ravel()
m_tr_slp = eval_metrics(y_train, prob_tr)
m_va_slp = eval_metrics(y_val, prob_va)
m_te_slp = eval_metrics(y_test, prob_te)

lr = LogisticRegression(max_iter=1000, random_state=SEED)
lr.fit(X_train_std, y_train)
prob_tr_lr = lr.predict_proba(X_train_std)[:,1]
prob_va_lr = lr.predict_proba(X_val_std)[:,1]
prob_te_lr = lr.predict_proba(X_test_std)[:,1]
m_tr_lr = eval_metrics(y_train, prob_tr_lr)
m_va_lr = eval_metrics(y_val, prob_va_lr)
m_te_lr = eval_metrics(y_test, prob_te_lr)

df = pd.DataFrame([
    {"model":"Perceptron","split":"train",**m_tr_slp},
    {"model":"Perceptron","split":"val",**m_va_slp},
    {"model":"Perceptron","split":"test",**m_te_slp},
    {"model":"LogisticRegression","split":"train",**m_tr_lr},
    {"model":"LogisticRegression","split":"val",**m_va_lr},
    {"model":"LogisticRegression","split":"test",**m_te_lr},
])
df.to_csv("metrics_breast_cancer.csv", index=False)

fpr, tpr, _ = roc_curve(y_test, prob_te)
auc_val = roc_auc_score(y_test, prob_te)
plt.figure(); plt.plot(fpr,tpr,label=f"SLP ROC (AUC={auc_val:.3f})"); plt.plot([0,1],[0,1],linestyle="--")
plt.xlabel("False Positive Rate"); plt.ylabel("True Positive Rate"); plt.title("ROC Curve – Perceptron (Test)"); plt.legend()
plt.savefig("roc_curve_slp.png", bbox_inches="tight"); plt.close()

cm = confusion_matrix(y_test, (prob_te>=0.5).astype(int))
plt.figure(); plt.imshow(cm, interpolation="nearest"); plt.title("Confusion Matrix – Perceptron (Test, thr=0.5)"); plt.colorbar()
ticks = np.arange(2); plt.xticks(ticks, ds.target_names, rotation=45); plt.yticks(ticks, ds.target_names)
for i in range(cm.shape[0]):
    for j in range(cm.shape[1]): plt.text(j,i,str(cm[i,j]),ha="center",va="center")
plt.ylabel("True label"); plt.xlabel("Predicted label"); plt.tight_layout()
plt.savefig("confusion_matrix_slp.png", bbox_inches="tight"); plt.close()

coefs = lr.coef_.ravel(); abs_coefs = np.abs(coefs); idx = np.argsort(-abs_coefs)[:10]
feat = np.array(ds.feature_names)[idx]; vals = coefs[idx]
plt.figure(); plt.barh(range(len(idx)), vals); plt.yticks(range(len(idx)), feat); plt.gca().invert_yaxis()
plt.xlabel("Coefficient"); plt.title("Top-10 |Coefficient| – Logistic Regression"); plt.tight_layout()
plt.savefig("feature_importance_lr_top10.png", bbox_inches="tight"); plt.close()

print("Done. Files: metrics_breast_cancer.csv, roc_curve_slp.png, confusion_matrix_slp.png, feature_importance_lr_top10.png")
