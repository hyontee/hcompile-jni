package com.blackhub.bronline.game.gui.familysystem;

import android.annotation.SuppressLint;
import android.content.Context;
import android.text.Editable;
import android.text.TextWatcher;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Filter;
import android.widget.FrameLayout;
import android.widget.TextView;
import androidx.compose.runtime.internal.StabilityInferred;
import androidx.constraintlayout.widget.ConstraintLayout;
import androidx.media3.extractor.text.ttml.TtmlNode;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;
import com.blackhub.bronline.R;
import com.blackhub.bronline.databinding.FamiliesFamilyControlBinding;
import com.blackhub.bronline.game.common.UILayout;
import com.blackhub.bronline.game.core.JNIActivity;
import com.blackhub.bronline.game.core.extension.IntExtensionKt;
import com.blackhub.bronline.game.gui.Useful;
import com.blackhub.bronline.game.gui.entertainmentsystem.adapters.EntertainmentSystemGamesFilterAdapter;
import com.blackhub.bronline.game.gui.entertainmentsystem.data.EntertainmentGamesFilter;
import com.blackhub.bronline.game.gui.familysystem.adapters.FamilyPlayersActionAdapter;
import com.blackhub.bronline.game.gui.familysystem.adapters.FamilyPlayersListAdapter;
import com.blackhub.bronline.game.gui.familysystem.data.FamilyPlayer;
import com.blackhub.bronline.game.gui.familysystem.data.FamilyPlayersAction;
import com.blackhub.bronline.game.gui.familysystem.network.ActionsWithJSON;
import java.util.ArrayList;
import java.util.Comparator;
import java.util.Iterator;
import java.util.List;
import kotlin.Metadata;
import kotlin.Unit;
import kotlin.collections.CollectionsKt___CollectionsKt;
import kotlin.comparisons.ComparisonsKt__ComparisonsKt;
import kotlin.jvm.functions.Function2;
import kotlin.jvm.internal.Intrinsics;
import kotlin.jvm.internal.SourceDebugExtension;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

/* compiled from: UIMenuFamilyPlayersSetting.kt */
@StabilityInferred(parameters = 0)
@Metadata(d1 = {"\u0000ª\u0001\n\u0002\u0018\u0002\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0002\b\u0002\n\u0002\u0010!\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0002\u0018\u0002\n\u0002\u0018\u0002\n\u0002\b\u0002\n\u0002\u0010\b\n\u0000\n\u0002\u0010\u0002\n\u0002\u0018\u0002\n\u0000\n\u0002\u0010\u000e\n\u0002\b\u0002\n\u0002\u0018\u0002\n\u0002\b\u0002\n\u0002\u0010\u0011\n\u0002\b\u0002\n\u0002\u0010\t\n\u0002\b\u0003\n\u0002\u0018\u0002\n\u0002\b\u0002\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0002\b\b\n\u0002\u0018\u0002\n\u0002\b\u0002\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0002\b\b\n\u0002\u0010 \n\u0002\b\u0011\n\u0002\u0010\u000b\n\u0002\b\t\b\u0007\u0018\u00002\u00020\u0001B\u0019\u0012\b\u0010\u0002\u001a\u0004\u0018\u00010\u0003\u0012\b\u0010\u0004\u001a\u0004\u0018\u00010\u0005¢\u0006\u0002\u0010\u0006J&\u0010,\u001a\u00020\u00142\f\u0010-\u001a\b\u0012\u0004\u0012\u00020\t0\b2\u0006\u0010.\u001a\u00020\u00172\u0006\u0010/\u001a\u00020\u0012H\u0002J\b\u00100\u001a\u00020\u0014H\u0002J\u0010\u00101\u001a\u00020\u00122\u0006\u00102\u001a\u00020\u0017H\u0002J\b\u00103\u001a\u000204H\u0016J\u001a\u00105\u001a\u0002042\u0006\u00106\u001a\u0002072\b\u00108\u001a\u0004\u0018\u000109H\u0016J\b\u0010:\u001a\u00020\u0014H\u0016J\b\u0010;\u001a\u00020\u0014H\u0016J\b\u0010<\u001a\u00020\u0014H\u0002J\b\u0010=\u001a\u00020\u0014H\u0002J\b\u0010>\u001a\u00020\u0014H\u0002J\u0014\u0010?\u001a\u00020\u00142\f\u0010@\u001a\b\u0012\u0004\u0012\u00020\t0\bJ\u0016\u0010A\u001a\b\u0012\u0004\u0012\u00020\t0B2\u0006\u0010C\u001a\u00020\u0012H\u0002J\u0016\u0010D\u001a\u00020\u00142\f\u0010E\u001a\b\u0012\u0004\u0012\u00020\t0BH\u0002J\b\u0010F\u001a\u00020\u0014H\u0002J\b\u0010G\u001a\u00020\u0014H\u0002J\b\u0010H\u001a\u00020\u0014H\u0002J\b\u0010I\u001a\u00020\u0014H\u0002J\b\u0010J\u001a\u00020\u0014H\u0002J\u0006\u0010K\u001a\u00020\u0014J\b\u0010L\u001a\u00020\u0014H\u0002J#\u0010M\u001a\u00020\u00142\f\u0010N\u001a\b\u0012\u0004\u0012\u00020\u00120\u001d2\b\u0010O\u001a\u0004\u0018\u00010\u0017¢\u0006\u0002\u0010PJ\u0006\u0010Q\u001a\u00020\u0014J\u0010\u0010R\u001a\u00020\u00142\u0006\u0010S\u001a\u00020TH\u0002J\u0010\u0010U\u001a\u00020\u00142\u0006\u0010V\u001a\u00020\u0012H\u0002J\u0010\u0010W\u001a\u00020\u00142\u0006\u0010X\u001a\u00020TH\u0002J\u0016\u0010Y\u001a\u00020\u00142\f\u0010Z\u001a\b\u0012\u0004\u0012\u00020\t0BH\u0002J\u0006\u0010[\u001a\u00020\u0014J\u0018\u0010\\\u001a\u00020\u00142\u0006\u00102\u001a\u00020\u00172\u0006\u0010/\u001a\u00020\u0012H\u0002R\u0010\u0010\u0004\u001a\u0004\u0018\u00010\u0005X\u0082\u0004¢\u0006\u0002\n\u0000R\u0014\u0010\u0007\u001a\b\u0012\u0004\u0012\u00020\t0\bX\u0082\u000e¢\u0006\u0002\n\u0000R\u000e\u0010\n\u001a\u00020\u000bX\u0082.¢\u0006\u0002\n\u0000RF\u0010\f\u001a:\u0012\u0013\u0012\u00110\u000e¢\u0006\f\b\u000f\u0012\b\b\u0010\u0012\u0004\b\b(\u0011\u0012\u0013\u0012\u00110\u0012¢\u0006\f\b\u000f\u0012\b\b\u0010\u0012\u0004\b\b(\u0013\u0012\u0004\u0012\u00020\u0014\u0018\u00010\rj\u0004\u0018\u0001`\u0015X\u0082\u000e¢\u0006\u0002\n\u0000R\u000e\u0010\u0016\u001a\u00020\u0017X\u0082\u000e¢\u0006\u0002\n\u0000R\u0014\u0010\u0018\u001a\b\u0012\u0004\u0012\u00020\t0\bX\u0082\u0004¢\u0006\u0002\n\u0000R\u0010\u0010\u0019\u001a\u0004\u0018\u00010\u001aX\u0082\u000e¢\u0006\u0002\n\u0000R\u000e\u0010\u001b\u001a\u00020\u0017X\u0082\u000e¢\u0006\u0002\n\u0000R\u0010\u0010\u0002\u001a\u0004\u0018\u00010\u0003X\u0082\u0004¢\u0006\u0002\n\u0000R\u0016\u0010\u001c\u001a\b\u0012\u0004\u0012\u00020\u00120\u001dX\u0082\u000e¢\u0006\u0004\n\u0002\u0010\u001eR\u000e\u0010\u001f\u001a\u00020 X\u0082\u000e¢\u0006\u0002\n\u0000RF\u0010!\u001a:\u0012\u0013\u0012\u00110\t¢\u0006\f\b\u000f\u0012\b\b\u0010\u0012\u0004\b\b(\"\u0012\u0013\u0012\u00110\u0012¢\u0006\f\b\u000f\u0012\b\b\u0010\u0012\u0004\b\b(#\u0012\u0004\u0012\u00020\u0014\u0018\u00010\rj\u0004\u0018\u0001`$X\u0082\u000e¢\u0006\u0002\n\u0000R\u0014\u0010%\u001a\b\u0012\u0004\u0012\u00020\u000e0\bX\u0082\u0004¢\u0006\u0002\n\u0000R\u0014\u0010&\u001a\b\u0012\u0004\u0012\u00020'0\bX\u0082\u0004¢\u0006\u0002\n\u0000R\u0010\u0010(\u001a\u0004\u0018\u00010)X\u0082\u000e¢\u0006\u0002\n\u0000R\u0010\u0010*\u001a\u0004\u0018\u00010+X\u0082\u000e¢\u0006\u0002\n\u0000¨\u0006]"}, d2 = {"Lcom/blackhub/bronline/game/gui/familysystem/UIMenuFamilyPlayersSetting;", "Lcom/blackhub/bronline/game/common/UILayout;", "mainActivity", "Lcom/blackhub/bronline/game/core/JNIActivity;", "actionsWithJSON", "Lcom/blackhub/bronline/game/gui/familysystem/network/ActionsWithJSON;", "(Lcom/blackhub/bronline/game/core/JNIActivity;Lcom/blackhub/bronline/game/gui/familysystem/network/ActionsWithJSON;)V", "allPlayersList", "", "Lcom/blackhub/bronline/game/gui/familysystem/data/FamilyPlayer;", "binding", "Lcom/blackhub/bronline/databinding/FamiliesFamilyControlBinding;", "clickActionsButton", "Lkotlin/Function2;", "Lcom/blackhub/bronline/game/gui/familysystem/data/FamilyPlayersAction;", "Lkotlin/ParameterName;", "name", "thisAction", "", "action", "", "Lcom/blackhub/bronline/game/gui/familysystem/OnClickActionsButton;", "currentPlayer", "", "currentPlayersList", "familyPlayersActionAdapter", "Lcom/blackhub/bronline/game/gui/familysystem/adapters/FamilyPlayersActionAdapter;", "getPlayerSearch", "oldAction", "", "[Ljava/lang/Integer;", "oldTimerPlayersClick", "", "playerClick", "item", "position", "Lcom/blackhub/bronline/game/gui/familysystem/OnClickPlayerList;", "playersActions", "playersFilter", "Lcom/blackhub/bronline/game/gui/entertainmentsystem/data/EntertainmentGamesFilter;", "playersFilterAdapter", "Lcom/blackhub/bronline/game/gui/entertainmentsystem/adapters/EntertainmentSystemGamesFilterAdapter;", "playersListAdapter", "Lcom/blackhub/bronline/game/gui/familysystem/adapters/FamilyPlayersListAdapter;", "actionWithPlayer", "currentCollection", "currentPlayersNick", "getCurrentAction", "clearInfoAboutPlayer", "getCurrentPlayersPosition", "getCurrentPlayer", "getView", "Landroid/view/View;", "onCreateView", "inflater", "Landroid/view/LayoutInflater;", TtmlNode.RUBY_CONTAINER, "Landroid/view/ViewGroup;", "onLayoutClose", "onLayoutShown", "removePlayerFromAllLists", "setActions", "setActionsDataInView", "setAllPlayers", "allPlayers", "setCurrentPlayersFilterList", "", "currentFilter", "setCurrentPlayersList", "thisList", "setFilter", "setFiltersDataInView", "setLogicForClickActionsButton", "setLogicForPlayerClick", "setLogicForPlayersFilterClick", "setNullableParameters", "setPlayersDataInView", "setPlayersInfo", "playersStates", "playersRankName", "([Ljava/lang/Integer;Ljava/lang/String;)V", "setStartParameters", "setVisibleForFilter", "ifSetVisible", "", "setVisibleForPlayersInfo", "setVisible", "setVisibleSearch", "ifShow", "updateCopyList", "playersList", "updatePlayersAction", "updatePlayersRank", "app_siteRelease"}, k = 1, mv = {1, 9, 0}, xi = 48)
@SuppressLint({"NotifyDataSetChanged"})
@SourceDebugExtension({"SMAP\nUIMenuFamilyPlayersSetting.kt\nKotlin\n*S Kotlin\n*F\n+ 1 UIMenuFamilyPlayersSetting.kt\ncom/blackhub/bronline/game/gui/familysystem/UIMenuFamilyPlayersSetting\n+ 2 _Collections.kt\nkotlin/collections/CollectionsKt___CollectionsKt\n*L\n1#1,570:1\n1045#2:571\n1045#2:572\n1054#2:573\n350#2,7:574\n*S KotlinDebug\n*F\n+ 1 UIMenuFamilyPlayersSetting.kt\ncom/blackhub/bronline/game/gui/familysystem/UIMenuFamilyPlayersSetting\n*L\n279#1:571\n297#1:572\n301#1:573\n406#1:574,7\n*E\n"})
/* loaded from: classes3.dex */
public final class UIMenuFamilyPlayersSetting extends UILayout {
    public static final int $stable = 8;

    @Nullable
    public final ActionsWithJSON actionsWithJSON;
    public FamiliesFamilyControlBinding binding;

    @Nullable
    public Function2<? super FamilyPlayersAction, ? super Integer, Unit> clickActionsButton;

    @Nullable
    public FamilyPlayersActionAdapter familyPlayersActionAdapter;

    @Nullable
    public final JNIActivity mainActivity;
    public long oldTimerPlayersClick;

    @Nullable
    public Function2<? super FamilyPlayer, ? super Integer, Unit> playerClick;

    @Nullable
    public EntertainmentSystemGamesFilterAdapter playersFilterAdapter;

    @Nullable
    public FamilyPlayersListAdapter playersListAdapter;

    @NotNull
    public List<FamilyPlayer> allPlayersList = new ArrayList();

    @NotNull
    public final List<FamilyPlayer> currentPlayersList = new ArrayList();

    @NotNull
    public final List<EntertainmentGamesFilter> playersFilter = new ArrayList();

    @NotNull
    public final List<FamilyPlayersAction> playersActions = new ArrayList();

    @NotNull
    public Integer[] oldAction = {-1, -1};

    @NotNull
    public String getPlayerSearch = "";

    @NotNull
    public String currentPlayer = "";

    @Override // com.blackhub.bronline.game.common.UILayout
    public void onLayoutClose() {
    }

    @Override // com.blackhub.bronline.game.common.UILayout
    public void onLayoutShown() {
    }

    public UIMenuFamilyPlayersSetting(@Nullable JNIActivity jNIActivity, @Nullable ActionsWithJSON actionsWithJSON) {
        this.mainActivity = jNIActivity;
        this.actionsWithJSON = actionsWithJSON;
    }

    public final void setStartParameters() {
        if (this.playersFilter.size() == 4) {
            this.playersFilter.get(2).setClicked(true);
            EntertainmentSystemGamesFilterAdapter entertainmentSystemGamesFilterAdapter = this.playersFilterAdapter;
            if (entertainmentSystemGamesFilterAdapter != null) {
                entertainmentSystemGamesFilterAdapter.notifyItemChanged(2);
                entertainmentSystemGamesFilterAdapter.setCheckOnlyElement(2);
            }
        }
        setCurrentPlayersList(setCurrentPlayersFilterList(3));
        FamiliesFamilyControlBinding familiesFamilyControlBinding = this.binding;
        if (familiesFamilyControlBinding == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
            familiesFamilyControlBinding = null;
        }
        familiesFamilyControlBinding.editTextSearch.getText().clear();
        setVisibleSearch(false);
        if (this.familyPlayersActionAdapter == null) {
            setActionsDataInView();
        }
    }

    @Override // com.blackhub.bronline.game.common.UILayout
    @NotNull
    /* renamed from: getView */
    public View getMViewRoot() {
        FamiliesFamilyControlBinding familiesFamilyControlBinding = this.binding;
        if (familiesFamilyControlBinding == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
            familiesFamilyControlBinding = null;
        }
        ConstraintLayout root = familiesFamilyControlBinding.getRoot();
        Intrinsics.checkNotNullExpressionValue(root, "getRoot(...)");
        return root;
    }

    @Override // com.blackhub.bronline.game.common.UILayout
    @NotNull
    public View onCreateView(@NotNull LayoutInflater inflater, @Nullable ViewGroup r5) {
        Intrinsics.checkNotNullParameter(inflater, "inflater");
        FamiliesFamilyControlBinding inflate = FamiliesFamilyControlBinding.inflate(inflater);
        Intrinsics.checkNotNullExpressionValue(inflate, "inflate(...)");
        this.binding = inflate;
        setLogicForPlayerClick();
        setPlayersDataInView();
        setFilter();
        setActions();
        setFiltersDataInView();
        setLogicForClickActionsButton();
        FamiliesFamilyControlBinding familiesFamilyControlBinding = this.binding;
        FamiliesFamilyControlBinding familiesFamilyControlBinding2 = null;
        if (familiesFamilyControlBinding == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
            familiesFamilyControlBinding = null;
        }
        familiesFamilyControlBinding.buttonPlayersFilter.setOnClickListener(new View.OnClickListener() { // from class: com.blackhub.bronline.game.gui.familysystem.UIMenuFamilyPlayersSetting$$ExternalSyntheticLambda0
            public /* synthetic */ UIMenuFamilyPlayersSetting$$ExternalSyntheticLambda0() {
            }

            @Override // android.view.View.OnClickListener
            public final void onClick(View view) {
                UIMenuFamilyPlayersSetting.onCreateView$lambda$1(UIMenuFamilyPlayersSetting.this, view);
            }
        });
        FamiliesFamilyControlBinding familiesFamilyControlBinding3 = this.binding;
        if (familiesFamilyControlBinding3 == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
            familiesFamilyControlBinding3 = null;
        }
        familiesFamilyControlBinding3.buttonPlayersSearch.setOnClickListener(new View.OnClickListener() { // from class: com.blackhub.bronline.game.gui.familysystem.UIMenuFamilyPlayersSetting$$ExternalSyntheticLambda1
            public /* synthetic */ UIMenuFamilyPlayersSetting$$ExternalSyntheticLambda1() {
            }

            @Override // android.view.View.OnClickListener
            public final void onClick(View view) {
                UIMenuFamilyPlayersSetting.onCreateView$lambda$2(UIMenuFamilyPlayersSetting.this, view);
            }
        });
        FamiliesFamilyControlBinding familiesFamilyControlBinding4 = this.binding;
        if (familiesFamilyControlBinding4 == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
            familiesFamilyControlBinding4 = null;
        }
        familiesFamilyControlBinding4.editTextSearch.addTextChangedListener(new TextWatcher() { // from class: com.blackhub.bronline.game.gui.familysystem.UIMenuFamilyPlayersSetting$onCreateView$3
            @Override // android.text.TextWatcher
            public void afterTextChanged(@Nullable Editable p0) {
            }

            @Override // android.text.TextWatcher
            public void beforeTextChanged(@Nullable CharSequence p0, int p1, int p2, int p3) {
            }

            public UIMenuFamilyPlayersSetting$onCreateView$3() {
            }

            @Override // android.text.TextWatcher
            public void onTextChanged(@Nullable CharSequence p0, int p1, int p2, int p3) {
                FamilyPlayersListAdapter familyPlayersListAdapter;
                Filter filter;
                String str;
                if (p0 != null) {
                    UIMenuFamilyPlayersSetting.this.getPlayerSearch = p0.toString();
                }
                familyPlayersListAdapter = UIMenuFamilyPlayersSetting.this.playersListAdapter;
                if (familyPlayersListAdapter == null || (filter = familyPlayersListAdapter.getFilter()) == null) {
                    return;
                }
                str = UIMenuFamilyPlayersSetting.this.getPlayerSearch;
                filter.filter(str);
            }
        });
        FamiliesFamilyControlBinding familiesFamilyControlBinding5 = this.binding;
        if (familiesFamilyControlBinding5 == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
            familiesFamilyControlBinding5 = null;
        }
        familiesFamilyControlBinding5.buttonBackToMenu.setOnClickListener(new View.OnClickListener() { // from class: com.blackhub.bronline.game.gui.familysystem.UIMenuFamilyPlayersSetting$$ExternalSyntheticLambda2
            public /* synthetic */ UIMenuFamilyPlayersSetting$$ExternalSyntheticLambda2() {
            }

            @Override // android.view.View.OnClickListener
            public final void onClick(View view) {
                UIMenuFamilyPlayersSetting.onCreateView$lambda$3(UIMenuFamilyPlayersSetting.this, view);
            }
        });
        FamiliesFamilyControlBinding familiesFamilyControlBinding6 = this.binding;
        if (familiesFamilyControlBinding6 == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
            familiesFamilyControlBinding6 = null;
        }
        familiesFamilyControlBinding6.titleLeftBlock.setSelected(true);
        familiesFamilyControlBinding6.playersPositionValue.setSelected(true);
        familiesFamilyControlBinding6.phoneNumberValue.setSelected(true);
        FamiliesFamilyControlBinding familiesFamilyControlBinding7 = this.binding;
        if (familiesFamilyControlBinding7 == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
        } else {
            familiesFamilyControlBinding2 = familiesFamilyControlBinding7;
        }
        ConstraintLayout root = familiesFamilyControlBinding2.getRoot();
        Intrinsics.checkNotNullExpressionValue(root, "getRoot(...)");
        return root;
    }

    public static final void onCreateView$lambda$1(UIMenuFamilyPlayersSetting this$0, View view) {
        Intrinsics.checkNotNullParameter(this$0, "this$0");
        FamiliesFamilyControlBinding familiesFamilyControlBinding = this$0.binding;
        if (familiesFamilyControlBinding == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
            familiesFamilyControlBinding = null;
        }
        if (familiesFamilyControlBinding.playersFilterList.getVisibility() == 0) {
            this$0.setVisibleForFilter(false);
        } else {
            this$0.setVisibleForFilter(true);
        }
    }

    public static final void onCreateView$lambda$2(UIMenuFamilyPlayersSetting this$0, View view) {
        Intrinsics.checkNotNullParameter(this$0, "this$0");
        FamiliesFamilyControlBinding familiesFamilyControlBinding = this$0.binding;
        FamiliesFamilyControlBinding familiesFamilyControlBinding2 = null;
        if (familiesFamilyControlBinding == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
            familiesFamilyControlBinding = null;
        }
        if (familiesFamilyControlBinding.editTextSearch.getVisibility() == 0) {
            FamiliesFamilyControlBinding familiesFamilyControlBinding3 = this$0.binding;
            if (familiesFamilyControlBinding3 == null) {
                Intrinsics.throwUninitializedPropertyAccessException("binding");
            } else {
                familiesFamilyControlBinding2 = familiesFamilyControlBinding3;
            }
            familiesFamilyControlBinding2.editTextSearch.getText().clear();
            this$0.setVisibleSearch(false);
            return;
        }
        this$0.setVisibleSearch(true);
    }

    public static final void onCreateView$lambda$3(UIMenuFamilyPlayersSetting this$0, View view) {
        Intrinsics.checkNotNullParameter(this$0, "this$0");
        ActionsWithJSON actionsWithJSON = this$0.actionsWithJSON;
        if (actionsWithJSON != null) {
            actionsWithJSON.sendGetLayout(5);
        }
    }

    public final void setLogicForPlayerClick() {
        this.playerClick = new Function2<FamilyPlayer, Integer, Unit>() { // from class: com.blackhub.bronline.game.gui.familysystem.UIMenuFamilyPlayersSetting$setLogicForPlayerClick$1
            public UIMenuFamilyPlayersSetting$setLogicForPlayerClick$1() {
            }

            @Override // kotlin.jvm.functions.Function2
            public /* bridge */ /* synthetic */ Unit invoke(FamilyPlayer familyPlayer, Integer num) {
                invoke(familyPlayer, num.intValue());
                return Unit.INSTANCE;
            }

            public void invoke(@NotNull FamilyPlayer item, int position) {
                long j;
                ActionsWithJSON actionsWithJSON;
                Intrinsics.checkNotNullParameter(item, "item");
                long currentTimeMillis = System.currentTimeMillis();
                j = UIMenuFamilyPlayersSetting.this.oldTimerPlayersClick;
                if (currentTimeMillis - j > 500) {
                    UIMenuFamilyPlayersSetting.this.oldTimerPlayersClick = System.currentTimeMillis();
                    UIMenuFamilyPlayersSetting.this.currentPlayer = item.getPlayersNick();
                    actionsWithJSON = UIMenuFamilyPlayersSetting.this.actionsWithJSON;
                    if (actionsWithJSON != null) {
                        actionsWithJSON.getPlayersInfo(item.getPlayersNick());
                    }
                }
            }
        };
    }

    public final void setPlayersDataInView() {
        this.playersListAdapter = new FamilyPlayersListAdapter(this.currentPlayersList, this.playerClick, this.mainActivity);
        FamiliesFamilyControlBinding familiesFamilyControlBinding = this.binding;
        FamiliesFamilyControlBinding familiesFamilyControlBinding2 = null;
        if (familiesFamilyControlBinding == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
            familiesFamilyControlBinding = null;
        }
        RecyclerView recyclerView = familiesFamilyControlBinding.playersList;
        FamiliesFamilyControlBinding familiesFamilyControlBinding3 = this.binding;
        if (familiesFamilyControlBinding3 == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
        } else {
            familiesFamilyControlBinding2 = familiesFamilyControlBinding3;
        }
        recyclerView.setLayoutManager(new LinearLayoutManager(familiesFamilyControlBinding2.getRoot().getContext(), 1, false));
        recyclerView.setAdapter(this.playersListAdapter);
    }

    public final void setFilter() {
        List<EntertainmentGamesFilter> list = this.playersFilter;
        list.clear();
        list.add(new EntertainmentGamesFilter(0, "Только онлайн", false, 4, null));
        list.add(new EntertainmentGamesFilter(1, "Только оффлайн", false, 4, null));
        list.add(new EntertainmentGamesFilter(2, "Ранг по возрастанию", false, 4, null));
        list.add(new EntertainmentGamesFilter(3, "Ранг по убыванию", false, 4, null));
    }

    public final void setActions() {
        List<FamilyPlayersAction> list = this.playersActions;
        list.clear();
        list.add(new FamilyPlayersAction(0, "Изменить ранг", 0, 0));
        list.add(new FamilyPlayersAction(1, "Управление выговорами", 0, 0));
        list.add(new FamilyPlayersAction(2, "Выдать мут (минут)", 0, 0));
        list.add(new FamilyPlayersAction(3, "Выгнать", 0, 1));
    }

    public final void setVisibleForFilter(boolean ifSetVisible) {
        FamiliesFamilyControlBinding familiesFamilyControlBinding = this.binding;
        if (familiesFamilyControlBinding == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
            familiesFamilyControlBinding = null;
        }
        JNIActivity jNIActivity = this.mainActivity;
        if (jNIActivity != null) {
            if (ifSetVisible) {
                familiesFamilyControlBinding.playersFilterList.setVisibility(0);
                familiesFamilyControlBinding.buttonPlayersFilter.setBackgroundColor(jNIActivity.getResources().getColor(R.color.red, null));
            } else {
                familiesFamilyControlBinding.playersFilterList.setVisibility(4);
                familiesFamilyControlBinding.buttonPlayersFilter.setBackgroundColor(jNIActivity.getResources().getColor(2131100871, null));
            }
        }
    }

    public final void setFiltersDataInView() {
        List<EntertainmentGamesFilter> list = this.playersFilter;
        Context context = getContext();
        Intrinsics.checkNotNullExpressionValue(context, "getContext(...)");
        this.playersFilterAdapter = new EntertainmentSystemGamesFilterAdapter(list, context);
        setLogicForPlayersFilterClick();
        FamiliesFamilyControlBinding familiesFamilyControlBinding = this.binding;
        FamiliesFamilyControlBinding familiesFamilyControlBinding2 = null;
        if (familiesFamilyControlBinding == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
            familiesFamilyControlBinding = null;
        }
        RecyclerView recyclerView = familiesFamilyControlBinding.playersFilterList;
        FamiliesFamilyControlBinding familiesFamilyControlBinding3 = this.binding;
        if (familiesFamilyControlBinding3 == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
        } else {
            familiesFamilyControlBinding2 = familiesFamilyControlBinding3;
        }
        recyclerView.setLayoutManager(new LinearLayoutManager(familiesFamilyControlBinding2.getRoot().getContext(), 1, false));
        recyclerView.setAdapter(this.playersFilterAdapter);
    }

    public final void setLogicForPlayersFilterClick() {
        EntertainmentSystemGamesFilterAdapter entertainmentSystemGamesFilterAdapter = this.playersFilterAdapter;
        if (entertainmentSystemGamesFilterAdapter == null) {
            return;
        }
        entertainmentSystemGamesFilterAdapter.setFilterClickListener(new Function2<EntertainmentGamesFilter, Integer, Unit>() { // from class: com.blackhub.bronline.game.gui.familysystem.UIMenuFamilyPlayersSetting$setLogicForPlayersFilterClick$1
            public UIMenuFamilyPlayersSetting$setLogicForPlayersFilterClick$1() {
                super(2);
            }

            @Override // kotlin.jvm.functions.Function2
            public /* bridge */ /* synthetic */ Unit invoke(EntertainmentGamesFilter entertainmentGamesFilter, Integer num) {
                invoke(entertainmentGamesFilter, num.intValue());
                return Unit.INSTANCE;
            }

            public final void invoke(@NotNull EntertainmentGamesFilter game, int i) {
                EntertainmentSystemGamesFilterAdapter entertainmentSystemGamesFilterAdapter2;
                List currentPlayersFilterList;
                List currentPlayersFilterList2;
                List currentPlayersFilterList3;
                List currentPlayersFilterList4;
                Intrinsics.checkNotNullParameter(game, "game");
                game.setClicked(true);
                entertainmentSystemGamesFilterAdapter2 = UIMenuFamilyPlayersSetting.this.playersFilterAdapter;
                if (entertainmentSystemGamesFilterAdapter2 != null) {
                    entertainmentSystemGamesFilterAdapter2.notifyItemChanged(i);
                    entertainmentSystemGamesFilterAdapter2.setCheckOnlyElement(i);
                }
                int id = game.getId();
                if (id == 0) {
                    UIMenuFamilyPlayersSetting uIMenuFamilyPlayersSetting = UIMenuFamilyPlayersSetting.this;
                    currentPlayersFilterList = uIMenuFamilyPlayersSetting.setCurrentPlayersFilterList(0);
                    uIMenuFamilyPlayersSetting.setCurrentPlayersList(currentPlayersFilterList);
                } else if (id == 1) {
                    UIMenuFamilyPlayersSetting uIMenuFamilyPlayersSetting2 = UIMenuFamilyPlayersSetting.this;
                    currentPlayersFilterList2 = uIMenuFamilyPlayersSetting2.setCurrentPlayersFilterList(1);
                    uIMenuFamilyPlayersSetting2.setCurrentPlayersList(currentPlayersFilterList2);
                } else if (id == 2) {
                    UIMenuFamilyPlayersSetting uIMenuFamilyPlayersSetting3 = UIMenuFamilyPlayersSetting.this;
                    currentPlayersFilterList3 = uIMenuFamilyPlayersSetting3.setCurrentPlayersFilterList(2);
                    uIMenuFamilyPlayersSetting3.setCurrentPlayersList(currentPlayersFilterList3);
                } else if (id == 3) {
                    UIMenuFamilyPlayersSetting uIMenuFamilyPlayersSetting4 = UIMenuFamilyPlayersSetting.this;
                    currentPlayersFilterList4 = uIMenuFamilyPlayersSetting4.setCurrentPlayersFilterList(3);
                    uIMenuFamilyPlayersSetting4.setCurrentPlayersList(currentPlayersFilterList4);
                }
                UIMenuFamilyPlayersSetting.this.setVisibleForFilter(false);
            }
        });
    }

    public final void setVisibleSearch(boolean ifShow) {
        FamiliesFamilyControlBinding familiesFamilyControlBinding = this.binding;
        if (familiesFamilyControlBinding == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
            familiesFamilyControlBinding = null;
        }
        if (ifShow) {
            familiesFamilyControlBinding.buttonPlayersFilter.setVisibility(4);
            familiesFamilyControlBinding.playersFilterList.setVisibility(4);
            familiesFamilyControlBinding.titleLeftBlock.setVisibility(4);
            familiesFamilyControlBinding.buttonPlayersSearch.setImageResource(R.drawable.ic_close_old);
            familiesFamilyControlBinding.editTextSearch.setVisibility(0);
            return;
        }
        familiesFamilyControlBinding.buttonPlayersFilter.setVisibility(0);
        familiesFamilyControlBinding.titleLeftBlock.setVisibility(0);
        familiesFamilyControlBinding.buttonPlayersSearch.setImageResource(R.drawable.ic_loupe);
        familiesFamilyControlBinding.editTextSearch.setVisibility(4);
    }

    public final void setAllPlayers(@NotNull List<FamilyPlayer> allPlayers) {
        Intrinsics.checkNotNullParameter(allPlayers, "allPlayers");
        List<FamilyPlayer> list = this.allPlayersList;
        list.clear();
        list.addAll(allPlayers);
    }

    public final List<FamilyPlayer> setCurrentPlayersFilterList(int currentFilter) {
        ArrayList arrayList = new ArrayList();
        List<FamilyPlayer> sortedWith = CollectionsKt___CollectionsKt.sortedWith(this.allPlayersList, new Comparator() { // from class: com.blackhub.bronline.game.gui.familysystem.UIMenuFamilyPlayersSetting$setCurrentPlayersFilterList$$inlined$sortedBy$1
            /* JADX WARN: Multi-variable type inference failed */
            @Override // java.util.Comparator
            public final int compare(T t, T t2) {
                return ComparisonsKt__ComparisonsKt.compareValues(((FamilyPlayer) t).getPlayersNick(), ((FamilyPlayer) t2).getPlayersNick());
            }
        });
        if (currentFilter == 0) {
            for (FamilyPlayer familyPlayer : sortedWith) {
                if (familyPlayer.getPlayersStatus() == 1) {
                    arrayList.add(familyPlayer);
                }
            }
        } else if (currentFilter == 1) {
            for (FamilyPlayer familyPlayer2 : sortedWith) {
                if (familyPlayer2.getPlayersStatus() == 0) {
                    arrayList.add(familyPlayer2);
                }
            }
        } else if (currentFilter == 2) {
            arrayList.addAll(CollectionsKt___CollectionsKt.sortedWith(sortedWith, new Comparator() { // from class: com.blackhub.bronline.game.gui.familysystem.UIMenuFamilyPlayersSetting$setCurrentPlayersFilterList$$inlined$sortedBy$2
                /* JADX WARN: Multi-variable type inference failed */
                @Override // java.util.Comparator
                public final int compare(T t, T t2) {
                    return ComparisonsKt__ComparisonsKt.compareValues(Integer.valueOf(((FamilyPlayer) t).getPlayersRank()), Integer.valueOf(((FamilyPlayer) t2).getPlayersRank()));
                }
            }));
        } else if (currentFilter == 3) {
            arrayList.addAll(CollectionsKt___CollectionsKt.sortedWith(sortedWith, new Comparator() { // from class: com.blackhub.bronline.game.gui.familysystem.UIMenuFamilyPlayersSetting$setCurrentPlayersFilterList$$inlined$sortedByDescending$1
                /* JADX WARN: Multi-variable type inference failed */
                @Override // java.util.Comparator
                public final int compare(T t, T t2) {
                    return ComparisonsKt__ComparisonsKt.compareValues(Integer.valueOf(((FamilyPlayer) t2).getPlayersRank()), Integer.valueOf(((FamilyPlayer) t).getPlayersRank()));
                }
            }));
        }
        return arrayList;
    }

    public final void setCurrentPlayersList(List<FamilyPlayer> thisList) {
        List<FamilyPlayer> list = this.currentPlayersList;
        list.clear();
        list.addAll(thisList);
        if (this.currentPlayersList.size() > 0) {
            int size = this.currentPlayersList.size();
            for (int i = 0; i < size; i++) {
                FamilyPlayer familyPlayer = this.currentPlayersList.get(i);
                if (i == 0) {
                    familyPlayer.setClicked(true);
                    this.playersActions.get(0).setActionsValue(familyPlayer.getPlayersRank());
                    this.currentPlayer = familyPlayer.getPlayersNick();
                    ActionsWithJSON actionsWithJSON = this.actionsWithJSON;
                    if (actionsWithJSON != null) {
                        actionsWithJSON.getPlayersInfo(familyPlayer.getPlayersNick());
                    }
                    FamiliesFamilyControlBinding familiesFamilyControlBinding = this.binding;
                    FamiliesFamilyControlBinding familiesFamilyControlBinding2 = null;
                    if (familiesFamilyControlBinding == null) {
                        Intrinsics.throwUninitializedPropertyAccessException("binding");
                        familiesFamilyControlBinding = null;
                    }
                    familiesFamilyControlBinding.titleRightBlock.setText(familyPlayer.getPlayersNick());
                    FamiliesFamilyControlBinding familiesFamilyControlBinding3 = this.binding;
                    if (familiesFamilyControlBinding3 == null) {
                        Intrinsics.throwUninitializedPropertyAccessException("binding");
                    } else {
                        familiesFamilyControlBinding2 = familiesFamilyControlBinding3;
                    }
                    familiesFamilyControlBinding2.playersRankValue.setText(String.valueOf(familyPlayer.getPlayersRank()));
                } else if (familyPlayer.isClicked()) {
                    familyPlayer.setClicked(false);
                }
            }
        }
        FamilyPlayersListAdapter familyPlayersListAdapter = this.playersListAdapter;
        if (familyPlayersListAdapter != null) {
            familyPlayersListAdapter.notifyDataSetChanged();
        }
        updateCopyList(thisList);
    }

    public final void updateCopyList(List<FamilyPlayer> playersList) {
        FamilyPlayersListAdapter familyPlayersListAdapter = this.playersListAdapter;
        if (familyPlayersListAdapter != null) {
            List<FamilyPlayer> copyList = familyPlayersListAdapter.getCopyList();
            copyList.clear();
            copyList.addAll(playersList);
        }
    }

    public final void updatePlayersRank(String getCurrentPlayer, int getCurrentAction) {
        actionWithPlayer(this.allPlayersList, getCurrentPlayer, getCurrentAction);
    }

    public final void actionWithPlayer(List<FamilyPlayer> currentCollection, String currentPlayersNick, int getCurrentAction) {
        for (FamilyPlayer familyPlayer : currentCollection) {
            if (Intrinsics.areEqual(familyPlayer.getPlayersNick(), currentPlayersNick)) {
                if (getCurrentAction == 0) {
                    familyPlayer.setPlayersRank(familyPlayer.getPlayersRank() - 1);
                } else if (getCurrentAction == 1) {
                    familyPlayer.setPlayersRank(familyPlayer.getPlayersRank() + 1);
                }
                FamiliesFamilyControlBinding familiesFamilyControlBinding = this.binding;
                if (familiesFamilyControlBinding == null) {
                    Intrinsics.throwUninitializedPropertyAccessException("binding");
                    familiesFamilyControlBinding = null;
                }
                familiesFamilyControlBinding.playersRankValue.setText(String.valueOf(familyPlayer.getPlayersRank()));
                return;
            }
        }
    }

    public final int getCurrentPlayersPosition(String getCurrentPlayer) {
        if (this.currentPlayersList.size() <= 0) {
            return 0;
        }
        int size = this.currentPlayersList.size();
        for (int i = 0; i < size; i++) {
            if (Intrinsics.areEqual(this.currentPlayersList.get(i).getPlayersNick(), getCurrentPlayer)) {
                return i;
            }
        }
        return 0;
    }

    public final void setPlayersInfo(@NotNull Integer[] playersStates, @Nullable String playersRankName) {
        Intrinsics.checkNotNullParameter(playersStates, "playersStates");
        if (playersStates.length != 5 || playersRankName == null) {
            return;
        }
        setVisibleForPlayersInfo(0);
        FamiliesFamilyControlBinding familiesFamilyControlBinding = this.binding;
        if (familiesFamilyControlBinding == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
            familiesFamilyControlBinding = null;
        }
        Useful useful = Useful.INSTANCE;
        int intValue = playersStates[0].intValue();
        FrameLayout familySkin = familiesFamilyControlBinding.familySkin;
        Intrinsics.checkNotNullExpressionValue(familySkin, "familySkin");
        useful.renderSkin(2, 134, intValue, 1, familySkin);
        familiesFamilyControlBinding.playersPositionValue.setText(playersRankName);
        familiesFamilyControlBinding.playersLevelValue.setText(String.valueOf(playersStates[1].intValue()));
        TextView textView = familiesFamilyControlBinding.reprimandsValue;
        JNIActivity jNIActivity = this.mainActivity;
        textView.setText(jNIActivity != null ? jNIActivity.getString(R.string.common_number_current_number_max, playersStates[2], 3) : null);
        familiesFamilyControlBinding.phoneNumberValue.setText(String.valueOf(playersStates[3].intValue()));
        List<FamilyPlayersAction> list = this.playersActions;
        list.get(1).setActionsValue(playersStates[2].intValue());
        list.get(2).setActionsValue(playersStates[4].intValue());
        Iterator<FamilyPlayer> it = this.currentPlayersList.iterator();
        int i = 0;
        while (true) {
            if (!it.hasNext()) {
                i = -1;
                break;
            } else if (Intrinsics.areEqual(it.next().getPlayersNick(), this.currentPlayer)) {
                break;
            } else {
                i++;
            }
        }
        FamilyPlayer familyPlayer = (FamilyPlayer) CollectionsKt___CollectionsKt.getOrNull(this.currentPlayersList, i);
        if (familyPlayer != null) {
            familyPlayer.setClicked(true);
        }
        FamilyPlayersListAdapter familyPlayersListAdapter = this.playersListAdapter;
        if (familyPlayersListAdapter != null) {
            familyPlayersListAdapter.notifyItemChanged(i);
            familyPlayersListAdapter.setCheckOnlyElement(i);
        }
        FamiliesFamilyControlBinding familiesFamilyControlBinding2 = this.binding;
        if (familiesFamilyControlBinding2 == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
            familiesFamilyControlBinding2 = null;
        }
        familiesFamilyControlBinding2.titleRightBlock.setText(familyPlayer != null ? familyPlayer.getPlayersNick() : null);
        familiesFamilyControlBinding2.playersRankValue.setText(String.valueOf(familyPlayer != null ? Integer.valueOf(familyPlayer.getPlayersRank()) : null));
        this.playersActions.get(0).setActionsValue(IntExtensionKt.getOrZero(familyPlayer != null ? Integer.valueOf(familyPlayer.getPlayersRank()) : null));
        FamilyPlayersListAdapter familyPlayersListAdapter2 = this.playersListAdapter;
        if (familyPlayersListAdapter2 != null) {
            familyPlayersListAdapter2.notifyItemChanged(i);
        }
        FamilyPlayersActionAdapter familyPlayersActionAdapter = this.familyPlayersActionAdapter;
        if (familyPlayersActionAdapter != null) {
            familyPlayersActionAdapter.notifyDataSetChanged();
        }
    }

    public final void setLogicForClickActionsButton() {
        this.clickActionsButton = new Function2<FamilyPlayersAction, Integer, Unit>() { // from class: com.blackhub.bronline.game.gui.familysystem.UIMenuFamilyPlayersSetting$setLogicForClickActionsButton$1
            public UIMenuFamilyPlayersSetting$setLogicForClickActionsButton$1() {
            }

            @Override // kotlin.jvm.functions.Function2
            public /* bridge */ /* synthetic */ Unit invoke(FamilyPlayersAction familyPlayersAction, Integer num) {
                invoke(familyPlayersAction, num.intValue());
                return Unit.INSTANCE;
            }

            public void invoke(@NotNull FamilyPlayersAction thisAction, int action) {
                long j;
                ActionsWithJSON actionsWithJSON;
                Integer[] numArr;
                Integer[] numArr2;
                Intrinsics.checkNotNullParameter(thisAction, "thisAction");
                long currentTimeMillis = System.currentTimeMillis();
                j = UIMenuFamilyPlayersSetting.this.oldTimerPlayersClick;
                if (currentTimeMillis - j > 1000) {
                    UIMenuFamilyPlayersSetting.this.oldTimerPlayersClick = System.currentTimeMillis();
                    actionsWithJSON = UIMenuFamilyPlayersSetting.this.actionsWithJSON;
                    if (actionsWithJSON != null) {
                        actionsWithJSON.setActionWithPlayer(thisAction.getActionsId(), action);
                    }
                    numArr = UIMenuFamilyPlayersSetting.this.oldAction;
                    numArr[0] = Integer.valueOf(thisAction.getActionsId());
                    numArr2 = UIMenuFamilyPlayersSetting.this.oldAction;
                    numArr2[1] = Integer.valueOf(action);
                }
            }
        };
    }

    public final void setActionsDataInView() {
        this.familyPlayersActionAdapter = new FamilyPlayersActionAdapter(this.playersActions, this.clickActionsButton);
        FamiliesFamilyControlBinding familiesFamilyControlBinding = this.binding;
        FamiliesFamilyControlBinding familiesFamilyControlBinding2 = null;
        if (familiesFamilyControlBinding == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
            familiesFamilyControlBinding = null;
        }
        RecyclerView recyclerView = familiesFamilyControlBinding.actionWithPlayer;
        FamiliesFamilyControlBinding familiesFamilyControlBinding3 = this.binding;
        if (familiesFamilyControlBinding3 == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
        } else {
            familiesFamilyControlBinding2 = familiesFamilyControlBinding3;
        }
        recyclerView.setLayoutManager(new LinearLayoutManager(familiesFamilyControlBinding2.getRoot().getContext(), 1, false));
        recyclerView.setAdapter(this.familyPlayersActionAdapter);
    }

    public final void updatePlayersAction() {
        int size = this.playersActions.size();
        for (int i = 0; i < size; i++) {
            if (this.oldAction[0].intValue() == this.playersActions.get(i).getActionsId()) {
                int intValue = this.oldAction[1].intValue();
                if (intValue == 0) {
                    int actionsId = this.playersActions.get(i).getActionsId();
                    if (actionsId == 0) {
                        FamilyPlayersAction familyPlayersAction = this.playersActions.get(i);
                        familyPlayersAction.setActionsValue(familyPlayersAction.getActionsValue() - 1);
                        updatePlayersRank(this.currentPlayer, 0);
                        FamilyPlayersListAdapter familyPlayersListAdapter = this.playersListAdapter;
                        if (familyPlayersListAdapter != null) {
                            familyPlayersListAdapter.notifyItemChanged(getCurrentPlayersPosition(this.currentPlayer));
                        }
                    } else if (actionsId == 1) {
                        FamilyPlayersAction familyPlayersAction2 = this.playersActions.get(i);
                        familyPlayersAction2.setActionsValue(familyPlayersAction2.getActionsValue() - 1);
                        FamiliesFamilyControlBinding familiesFamilyControlBinding = this.binding;
                        if (familiesFamilyControlBinding == null) {
                            Intrinsics.throwUninitializedPropertyAccessException("binding");
                            familiesFamilyControlBinding = null;
                        }
                        TextView textView = familiesFamilyControlBinding.reprimandsValue;
                        JNIActivity jNIActivity = this.mainActivity;
                        textView.setText(jNIActivity != null ? jNIActivity.getString(R.string.common_number_current_number_max, Integer.valueOf(this.playersActions.get(i).getActionsValue()), 3) : null);
                    } else if (actionsId == 2) {
                        if (this.playersActions.get(i).getActionsValue() < 600) {
                            this.playersActions.get(i).setActionsValue(0);
                        } else {
                            FamilyPlayersAction familyPlayersAction3 = this.playersActions.get(i);
                            familyPlayersAction3.setActionsValue(familyPlayersAction3.getActionsValue() - 600);
                        }
                    }
                    FamilyPlayersActionAdapter familyPlayersActionAdapter = this.familyPlayersActionAdapter;
                    if (familyPlayersActionAdapter != null) {
                        familyPlayersActionAdapter.notifyItemChanged(i);
                        return;
                    }
                    return;
                }
                if (intValue != 1) {
                    if (intValue != 2) {
                        return;
                    }
                    removePlayerFromAllLists();
                    return;
                }
                int actionsId2 = this.playersActions.get(i).getActionsId();
                if (actionsId2 == 0) {
                    FamilyPlayersAction familyPlayersAction4 = this.playersActions.get(i);
                    familyPlayersAction4.setActionsValue(familyPlayersAction4.getActionsValue() + 1);
                    updatePlayersRank(this.currentPlayer, 1);
                    FamilyPlayersListAdapter familyPlayersListAdapter2 = this.playersListAdapter;
                    if (familyPlayersListAdapter2 != null) {
                        familyPlayersListAdapter2.notifyItemChanged(getCurrentPlayersPosition(this.currentPlayer));
                    }
                } else if (actionsId2 == 1) {
                    FamilyPlayersAction familyPlayersAction5 = this.playersActions.get(i);
                    familyPlayersAction5.setActionsValue(familyPlayersAction5.getActionsValue() + 1);
                    if (this.playersActions.get(i).getActionsValue() == 3) {
                        removePlayerFromAllLists();
                    } else {
                        FamiliesFamilyControlBinding familiesFamilyControlBinding2 = this.binding;
                        if (familiesFamilyControlBinding2 == null) {
                            Intrinsics.throwUninitializedPropertyAccessException("binding");
                            familiesFamilyControlBinding2 = null;
                        }
                        TextView textView2 = familiesFamilyControlBinding2.reprimandsValue;
                        JNIActivity jNIActivity2 = this.mainActivity;
                        textView2.setText(jNIActivity2 != null ? jNIActivity2.getString(R.string.common_number_current_number_max, Integer.valueOf(this.playersActions.get(i).getActionsValue()), 3) : null);
                    }
                } else if (actionsId2 == 2) {
                    if (this.playersActions.get(i).getActionsValue() > 3000) {
                        this.playersActions.get(i).setActionsValue(3600);
                    } else {
                        FamilyPlayersAction familyPlayersAction6 = this.playersActions.get(i);
                        familyPlayersAction6.setActionsValue(familyPlayersAction6.getActionsValue() + 600);
                    }
                }
                FamilyPlayersActionAdapter familyPlayersActionAdapter2 = this.familyPlayersActionAdapter;
                if (familyPlayersActionAdapter2 != null) {
                    familyPlayersActionAdapter2.notifyItemChanged(i);
                    return;
                }
                return;
            }
        }
    }

    public final void removePlayerFromAllLists() {
        int i = 0;
        if (this.allPlayersList.size() > 0) {
            int size = this.allPlayersList.size();
            int i2 = 0;
            while (true) {
                if (i2 >= size) {
                    break;
                }
                if (Intrinsics.areEqual(this.allPlayersList.get(i2).getPlayersNick(), this.currentPlayer)) {
                    this.allPlayersList.remove(i2);
                    break;
                }
                i2++;
            }
        }
        if (this.currentPlayersList.size() > 0) {
            int size2 = this.currentPlayersList.size();
            while (true) {
                if (i >= size2) {
                    break;
                }
                if (Intrinsics.areEqual(this.currentPlayersList.get(i).getPlayersNick(), this.currentPlayer)) {
                    this.currentPlayersList.remove(i);
                    FamilyPlayersListAdapter familyPlayersListAdapter = this.playersListAdapter;
                    if (familyPlayersListAdapter != null) {
                        familyPlayersListAdapter.notifyItemRemoved(i);
                    }
                } else {
                    i++;
                }
            }
        }
        updateCopyList(this.currentPlayersList);
        clearInfoAboutPlayer();
    }

    public final void clearInfoAboutPlayer() {
        setVisibleForPlayersInfo(4);
    }

    public final void setVisibleForPlayersInfo(int setVisible) {
        FamiliesFamilyControlBinding familiesFamilyControlBinding = this.binding;
        if (familiesFamilyControlBinding == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
            familiesFamilyControlBinding = null;
        }
        familiesFamilyControlBinding.playersInfo.setVisibility(setVisible);
        familiesFamilyControlBinding.actionWithPlayer.setVisibility(setVisible);
        familiesFamilyControlBinding.familySkin.setVisibility(setVisible);
        familiesFamilyControlBinding.titleRightBlock.setVisibility(setVisible);
    }

    public final void setNullableParameters() {
        this.playersFilterAdapter = null;
        this.playersListAdapter = null;
        this.playerClick = null;
        this.allPlayersList.clear();
        this.currentPlayersList.clear();
        this.playersFilter.clear();
        this.playersActions.clear();
        this.familyPlayersActionAdapter = null;
        this.clickActionsButton = null;
    }
}
