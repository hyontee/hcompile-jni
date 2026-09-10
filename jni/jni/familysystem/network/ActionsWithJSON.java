package com.blackhub.bronline.game.gui.familysystem.network;

import androidx.compose.runtime.internal.StabilityInferred;
import com.blackhub.bronline.game.GUIManager;
import com.fasterxml.jackson.databind.deser.std.ThrowableDeserializer;
import com.google.firebase.crashlytics.FirebaseCrashlytics;
import kotlin.Metadata;
import kotlin.jvm.internal.Intrinsics;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;
import org.json.JSONObject;

/* JADX WARN: Classes with same name are omitted:
  C:\javanew\classes3.dex
 */
/* compiled from: ActionsWithJSON.kt */
@StabilityInferred(parameters = 0)
@Metadata(d1 = {"\u0000(\n\u0002\u0018\u0002\n\u0002\u0010\u0000\n\u0000\n\u0002\u0018\u0002\n\u0002\b\u0002\n\u0002\u0010\u0002\n\u0000\n\u0002\u0010\b\n\u0002\b\n\n\u0002\u0010\u000e\n\u0002\b\u001d\b\u0007\u0018\u00002\u00020\u0001B\u000f\u0012\b\u0010\u0002\u001a\u0004\u0018\u00010\u0003¢\u0006\u0002\u0010\u0004J\u000e\u0010\u0005\u001a\u00020\u00062\u0006\u0010\u0007\u001a\u00020\bJ\u000e\u0010\t\u001a\u00020\u00062\u0006\u0010\n\u001a\u00020\bJ\u001e\u0010\u000b\u001a\u00020\u00062\u0006\u0010\f\u001a\u00020\b2\u0006\u0010\r\u001a\u00020\b2\u0006\u0010\u000e\u001a\u00020\bJ\u000e\u0010\u000f\u001a\u00020\u00062\u0006\u0010\u0010\u001a\u00020\bJ\u0016\u0010\u0011\u001a\u00020\u00062\u0006\u0010\f\u001a\u00020\b2\u0006\u0010\u0012\u001a\u00020\u0013J\u000e\u0010\u0014\u001a\u00020\u00062\u0006\u0010\u0015\u001a\u00020\u0013J\u0006\u0010\u0016\u001a\u00020\u0006J\u000e\u0010\u0017\u001a\u00020\u00062\u0006\u0010\u0018\u001a\u00020\bJ\u000e\u0010\u0019\u001a\u00020\u00062\u0006\u0010\u001a\u001a\u00020\bJ\u000e\u0010\u001b\u001a\u00020\u00062\u0006\u0010\u001c\u001a\u00020\bJ\u000e\u0010\u001d\u001a\u00020\u00062\u0006\u0010\u001e\u001a\u00020\u0013J\u000e\u0010\u001f\u001a\u00020\u00062\u0006\u0010 \u001a\u00020\u0013J\u000e\u0010!\u001a\u00020\u00062\u0006\u0010\u001c\u001a\u00020\bJ\u001e\u0010\"\u001a\u00020\u00062\u0006\u0010\u0010\u001a\u00020\b2\u0006\u0010#\u001a\u00020\b2\u0006\u0010$\u001a\u00020\bJ\u0016\u0010%\u001a\u00020\u00062\u0006\u0010&\u001a\u00020\b2\u0006\u0010#\u001a\u00020\bJ\u000e\u0010'\u001a\u00020\u00062\u0006\u0010\u0018\u001a\u00020\bJ\u000e\u0010(\u001a\u00020\u00062\u0006\u0010)\u001a\u00020\bJ\u000e\u0010*\u001a\u00020\u00062\u0006\u0010+\u001a\u00020\bJ\u000e\u0010,\u001a\u00020\u00062\u0006\u0010-\u001a\u00020\bJ\u000e\u0010.\u001a\u00020\u00062\u0006\u0010/\u001a\u00020\bR\u0010\u0010\u0002\u001a\u0004\u0018\u00010\u0003X\u0082\u0004¢\u0006\u0002\n\u0000¨\u00060"}, d2 = {"Lcom/blackhub/bronline/game/gui/familysystem/network/ActionsWithJSON;", "", "guiManager", "Lcom/blackhub/bronline/game/GUIManager;", "(Lcom/blackhub/bronline/game/GUIManager;)V", "buyItemInShop", "", "itemsId", "", "buyToken", "valueOfToken", "clickOnAccess", "ranksPosition", "accessPosition", "accessStatus", "clickOnCar", "modelId", "clickOnRank", "ranksName", "", "getPlayersInfo", "playersNick", "openLog", "openStore", "ifSafeOrStore", "sendDeleteNotification", "thisNotificationId", "sendGetLayout", "typeKey", "sendMessageError", ThrowableDeserializer.PROP_NAME_MESSAGE, "sendMessageForServer", "thisMessage", "sendPressButton", "setActionWithCar", "currentAction", "access", "setActionWithPlayer", "actionsType", "setClickOnStore", "setMenuPosition", "menuId", "setStartColor", "colorsPosition", "setUpgradeForFamily", "upgradeId", "startQuest", "questsId", "app_siteRelease"}, k = 1, mv = {1, 9, 0}, xi = 48)
/* loaded from: classes3.dex */
public final class ActionsWithJSON {
    public static final int $stable = 8;

    @Nullable
    public final GUIManager guiManager;

    public ActionsWithJSON(@Nullable GUIManager gUIManager) {
        this.guiManager = gUIManager;
    }

    public final void sendPressButton(int typeKey) {
        JSONObject jSONObject = new JSONObject();
        try {
            jSONObject.put("t", typeKey);
            GUIManager gUIManager = this.guiManager;
            if (gUIManager != null) {
                gUIManager.sendJsonData(45, jSONObject);
            }
        } catch (Exception e) {
            FirebaseCrashlytics.getInstance().recordException(e);
        }
    }

    public final void sendGetLayout(int typeKey) {
        JSONObject jSONObject = new JSONObject();
        try {
            jSONObject.put("t", typeKey);
            GUIManager gUIManager = this.guiManager;
            if (gUIManager != null) {
                gUIManager.sendJsonData(45, jSONObject);
            }
        } catch (Exception e) {
            FirebaseCrashlytics.getInstance().recordException(e);
        }
    }

    public final void startQuest(int questsId) {
        JSONObject jSONObject = new JSONObject();
        try {
            jSONObject.put("t", 1);
            jSONObject.put("id", questsId);
            GUIManager gUIManager = this.guiManager;
            if (gUIManager != null) {
                gUIManager.sendJsonData(45, jSONObject);
            }
        } catch (Exception e) {
            FirebaseCrashlytics.getInstance().recordException(e);
        }
    }

    public final void clickOnCar(int modelId) {
        JSONObject jSONObject = new JSONObject();
        try {
            jSONObject.put("t", 2);
            jSONObject.put("s", 1);
            jSONObject.put("id", modelId);
            GUIManager gUIManager = this.guiManager;
            if (gUIManager != null) {
                gUIManager.sendJsonData(45, jSONObject);
            }
        } catch (Exception e) {
            FirebaseCrashlytics.getInstance().recordException(e);
        }
    }

    public final void setActionWithCar(int modelId, int currentAction, int access) {
        JSONObject jSONObject = new JSONObject();
        try {
            jSONObject.put("t", 2);
            jSONObject.put("s", 2);
            jSONObject.put("m", modelId);
            jSONObject.put("id", currentAction);
            if (access != -1) {
                jSONObject.put("r", access);
            }
            GUIManager gUIManager = this.guiManager;
            if (gUIManager != null) {
                gUIManager.sendJsonData(45, jSONObject);
            }
        } catch (Exception e) {
            FirebaseCrashlytics.getInstance().recordException(e);
        }
    }

    public final void buyItemInShop(int itemsId) {
        JSONObject jSONObject = new JSONObject();
        try {
            jSONObject.put("t", 3);
            jSONObject.put("id", itemsId);
            GUIManager gUIManager = this.guiManager;
            if (gUIManager != null) {
                gUIManager.sendJsonData(45, jSONObject);
            }
        } catch (Exception e) {
            FirebaseCrashlytics.getInstance().recordException(e);
        }
    }

    public final void setUpgradeForFamily(int upgradeId) {
        JSONObject jSONObject = new JSONObject();
        try {
            jSONObject.put("t", 4);
            jSONObject.put("s", 1);
            jSONObject.put("id", upgradeId);
            GUIManager gUIManager = this.guiManager;
            if (gUIManager != null) {
                gUIManager.sendJsonData(45, jSONObject);
            }
        } catch (Exception e) {
            FirebaseCrashlytics.getInstance().recordException(e);
        }
    }

    public final void setMenuPosition(int menuId) {
        JSONObject jSONObject = new JSONObject();
        try {
            jSONObject.put("t", 5);
            jSONObject.put("id", menuId);
            GUIManager gUIManager = this.guiManager;
            if (gUIManager != null) {
                gUIManager.sendJsonData(45, jSONObject);
            }
        } catch (Exception e) {
            FirebaseCrashlytics.getInstance().recordException(e);
        }
    }

    public final void clickOnRank(int ranksPosition, @NotNull String ranksName) {
        Intrinsics.checkNotNullParameter(ranksName, "ranksName");
        JSONObject jSONObject = new JSONObject();
        try {
            jSONObject.put("t", 5);
            jSONObject.put("s", 1);
            jSONObject.put("id", 1);
            jSONObject.put("r", ranksPosition);
            jSONObject.put("n", ranksName);
            GUIManager gUIManager = this.guiManager;
            if (gUIManager != null) {
                gUIManager.sendJsonData(45, jSONObject);
            }
        } catch (Exception e) {
            FirebaseCrashlytics.getInstance().recordException(e);
        }
    }

    public final void getPlayersInfo(@NotNull String playersNick) {
        Intrinsics.checkNotNullParameter(playersNick, "playersNick");
        JSONObject jSONObject = new JSONObject();
        try {
            jSONObject.put("t", 5);
            jSONObject.put("s", 1);
            jSONObject.put("id", 2);
            jSONObject.put("n", playersNick);
            GUIManager gUIManager = this.guiManager;
            if (gUIManager != null) {
                gUIManager.sendJsonData(45, jSONObject);
            }
        } catch (Exception e) {
            FirebaseCrashlytics.getInstance().recordException(e);
        }
    }

    public final void clickOnAccess(int ranksPosition, int accessPosition, int accessStatus) {
        JSONObject jSONObject = new JSONObject();
        try {
            jSONObject.put("t", 5);
            jSONObject.put("s", 2);
            jSONObject.put("id", 1);
            jSONObject.put("r", ranksPosition);
            jSONObject.put("k", accessPosition);
            jSONObject.put("n", accessStatus);
            GUIManager gUIManager = this.guiManager;
            if (gUIManager != null) {
                gUIManager.sendJsonData(45, jSONObject);
            }
        } catch (Exception e) {
            FirebaseCrashlytics.getInstance().recordException(e);
        }
    }

    public final void setActionWithPlayer(int actionsType, int currentAction) {
        JSONObject jSONObject = new JSONObject();
        try {
            jSONObject.put("t", 5);
            jSONObject.put("s", 2);
            jSONObject.put("id", 2);
            jSONObject.put("r", actionsType);
            if (currentAction != 2) {
                jSONObject.put("k", currentAction);
            }
            GUIManager gUIManager = this.guiManager;
            if (gUIManager != null) {
                gUIManager.sendJsonData(45, jSONObject);
            }
        } catch (Exception e) {
            FirebaseCrashlytics.getInstance().recordException(e);
        }
    }

    public final void setStartColor(int colorsPosition) {
        JSONObject jSONObject = new JSONObject();
        try {
            jSONObject.put("t", 5);
            jSONObject.put("s", 3);
            jSONObject.put("id", 1);
            jSONObject.put("r", colorsPosition);
            GUIManager gUIManager = this.guiManager;
            if (gUIManager != null) {
                gUIManager.sendJsonData(45, jSONObject);
            }
        } catch (Exception e) {
            FirebaseCrashlytics.getInstance().recordException(e);
        }
    }

    public final void setClickOnStore(int ifSafeOrStore) {
        JSONObject jSONObject = new JSONObject();
        try {
            jSONObject.put("t", 5);
            jSONObject.put("id", 3);
            jSONObject.put("r", ifSafeOrStore);
            GUIManager gUIManager = this.guiManager;
            if (gUIManager != null) {
                gUIManager.sendJsonData(45, jSONObject);
            }
        } catch (Exception e) {
            FirebaseCrashlytics.getInstance().recordException(e);
        }
    }

    public final void openStore(int ifSafeOrStore) {
        JSONObject jSONObject = new JSONObject();
        try {
            jSONObject.put("t", 5);
            jSONObject.put("id", 3);
            jSONObject.put("r", ifSafeOrStore);
            jSONObject.put("s", 1);
            GUIManager gUIManager = this.guiManager;
            if (gUIManager != null) {
                gUIManager.sendJsonData(45, jSONObject);
            }
        } catch (Exception e) {
            FirebaseCrashlytics.getInstance().recordException(e);
        }
    }

    public final void openLog() {
        JSONObject jSONObject = new JSONObject();
        try {
            jSONObject.put("t", 5);
            jSONObject.put("id", 4);
            jSONObject.put("s", 1);
            GUIManager gUIManager = this.guiManager;
            if (gUIManager != null) {
                gUIManager.sendJsonData(45, jSONObject);
            }
        } catch (Exception e) {
            FirebaseCrashlytics.getInstance().recordException(e);
        }
    }

    public final void sendMessageForServer(@NotNull String thisMessage) {
        Intrinsics.checkNotNullParameter(thisMessage, "thisMessage");
        JSONObject jSONObject = new JSONObject();
        try {
            jSONObject.put("t", 6);
            jSONObject.put("b", 1);
            jSONObject.put("m", thisMessage);
            GUIManager gUIManager = this.guiManager;
            if (gUIManager != null) {
                gUIManager.sendJsonData(45, jSONObject);
            }
        } catch (Exception e) {
            FirebaseCrashlytics.getInstance().recordException(e);
        }
    }

    public final void sendDeleteNotification(int thisNotificationId) {
        JSONObject jSONObject = new JSONObject();
        try {
            jSONObject.put("t", 6);
            jSONObject.put("b", 2);
            jSONObject.put("x", thisNotificationId);
            GUIManager gUIManager = this.guiManager;
            if (gUIManager != null) {
                gUIManager.sendJsonData(45, jSONObject);
            }
        } catch (Exception e) {
            FirebaseCrashlytics.getInstance().recordException(e);
        }
    }

    public final void buyToken(int valueOfToken) {
        JSONObject jSONObject = new JSONObject();
        try {
            jSONObject.put("t", 9);
            jSONObject.put("s", 2);
            jSONObject.put("v", valueOfToken);
            GUIManager gUIManager = this.guiManager;
            if (gUIManager != null) {
                gUIManager.sendJsonData(45, jSONObject);
            }
        } catch (Exception e) {
            FirebaseCrashlytics.getInstance().recordException(e);
        }
    }

    public final void sendMessageError(@NotNull String message) {
        Intrinsics.checkNotNullParameter(message, "message");
        JSONObject jSONObject = new JSONObject();
        try {
            jSONObject.put("o", 1);
            jSONObject.put("t", 2);
            jSONObject.put("d", 2);
            jSONObject.put("i", message);
            jSONObject.put("s", -1);
            GUIManager gUIManager = this.guiManager;
            if (gUIManager != null) {
                gUIManager.onPacketIncoming(13, jSONObject);
            }
        } catch (Exception e) {
            FirebaseCrashlytics.getInstance().recordException(e);
        }
    }
}
