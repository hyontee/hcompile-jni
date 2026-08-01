package com.blackhub.bronline.game.gui.familysystem;

import android.text.SpannableString;
import android.text.method.ScrollingMovementMethod;
import android.text.style.ForegroundColorSpan;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.widget.TextView;
import androidx.compose.runtime.internal.StabilityInferred;
import androidx.constraintlayout.widget.ConstraintLayout;
import androidx.core.content.ContextCompat;
import androidx.media3.extractor.text.ttml.TtmlNode;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;
import com.blackhub.bronline.R;
import com.blackhub.bronline.databinding.FamiliesQuestsLayoutBinding;
import com.blackhub.bronline.game.common.UILayout;
import com.blackhub.bronline.game.core.JNIActivity;
import com.blackhub.bronline.game.gui.familysystem.adapters.FamilyQuestsAdapter;
import com.blackhub.bronline.game.gui.familysystem.data.FamilySystemList;
import com.blackhub.bronline.game.gui.familysystem.data.FamilySystemTasksObj;
import com.blackhub.bronline.game.gui.familysystem.network.ActionsWithJSON;
import java.util.ArrayList;
import java.util.List;
import kotlin.Metadata;
import kotlin.Unit;
import kotlin.jvm.functions.Function2;
import kotlin.jvm.internal.Intrinsics;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

/* compiled from: UIFamilyQuests.kt */
@StabilityInferred(parameters = 0)
@Metadata(d1 = {"\u0000v\n\u0002\u0018\u0002\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0002\b\u0002\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0002\u0018\u0002\n\u0002\u0018\u0002\n\u0002\b\u0002\n\u0002\u0010\b\n\u0000\n\u0002\u0010\u0002\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0000\n\u0002\u0010!\n\u0002\b\u0002\n\u0002\u0010\t\n\u0002\b\u0003\n\u0002\u0018\u0002\n\u0002\b\u0002\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0002\b\b\b\u0007\u0018\u00002\u00020\u0001B!\u0012\u0006\u0010\u0002\u001a\u00020\u0003\u0012\b\u0010\u0004\u001a\u0004\u0018\u00010\u0005\u0012\b\u0010\u0006\u001a\u0004\u0018\u00010\u0007¢\u0006\u0002\u0010\bJ\b\u0010\u001f\u001a\u00020\u0015H\u0002J\b\u0010 \u001a\u00020!H\u0016J\u001a\u0010\"\u001a\u00020!2\u0006\u0010#\u001a\u00020$2\b\u0010%\u001a\u0004\u0018\u00010&H\u0016J\b\u0010'\u001a\u00020\u0015H\u0016J\b\u0010(\u001a\u00020\u0015H\u0016J\b\u0010)\u001a\u00020\u0015H\u0002J\u0010\u0010*\u001a\u00020\u00152\u0006\u0010\u0012\u001a\u00020\u000fH\u0002J\b\u0010+\u001a\u00020\u0015H\u0002J\u0006\u0010,\u001a\u00020\u0015J\u0006\u0010-\u001a\u00020\u0015R\u0010\u0010\u0006\u001a\u0004\u0018\u00010\u0007X\u0082\u0004¢\u0006\u0002\n\u0000R\u0010\u0010\t\u001a\u0004\u0018\u00010\nX\u0082\u000e¢\u0006\u0002\n\u0000R\u000e\u0010\u000b\u001a\u00020\fX\u0082.¢\u0006\u0002\n\u0000R\u0010\u0010\u0004\u001a\u0004\u0018\u00010\u0005X\u0082\u0004¢\u0006\u0002\n\u0000R\u000e\u0010\u0002\u001a\u00020\u0003X\u0082\u0004¢\u0006\u0002\n\u0000RF\u0010\r\u001a:\u0012\u0013\u0012\u00110\u000f¢\u0006\f\b\u0010\u0012\b\b\u0011\u0012\u0004\b\b(\u0012\u0012\u0013\u0012\u00110\u0013¢\u0006\f\b\u0010\u0012\b\b\u0011\u0012\u0004\b\b(\u0014\u0012\u0004\u0012\u00020\u0015\u0018\u00010\u000ej\u0004\u0018\u0001`\u0016X\u0082\u000e¢\u0006\u0002\n\u0000R\u0010\u0010\u0017\u001a\u0004\u0018\u00010\u0018X\u0082\u000e¢\u0006\u0002\n\u0000R\u0014\u0010\u0019\u001a\b\u0012\u0004\u0012\u00020\u000f0\u001aX\u0082\u0004¢\u0006\u0002\n\u0000R\u000e\u0010\u001b\u001a\u00020\u0013X\u0082\u000e¢\u0006\u0002\n\u0000R\u000e\u0010\u001c\u001a\u00020\u001dX\u0082\u000e¢\u0006\u0002\n\u0000R\u000e\u0010\u001e\u001a\u00020\u001dX\u0082\u000e¢\u0006\u0002\n\u0000¨\u0006."}, d2 = {"Lcom/blackhub/bronline/game/gui/familysystem/UIFamilyQuests;", "Lcom/blackhub/bronline/game/common/UILayout;", "mainRoot", "Lcom/blackhub/bronline/game/gui/familysystem/GUIFamilySystem;", "mainActivity", "Lcom/blackhub/bronline/game/core/JNIActivity;", "actionsWithJSON", "Lcom/blackhub/bronline/game/gui/familysystem/network/ActionsWithJSON;", "(Lcom/blackhub/bronline/game/gui/familysystem/GUIFamilySystem;Lcom/blackhub/bronline/game/core/JNIActivity;Lcom/blackhub/bronline/game/gui/familysystem/network/ActionsWithJSON;)V", "anim", "Landroid/view/animation/Animation;", "binding", "Lcom/blackhub/bronline/databinding/FamiliesQuestsLayoutBinding;", "onClickQuestInFamilyQuestsList", "Lkotlin/Function2;", "Lcom/blackhub/bronline/game/gui/familysystem/data/FamilySystemTasksObj;", "Lkotlin/ParameterName;", "name", "thisItem", "", "position", "", "Lcom/blackhub/bronline/game/gui/familysystem/OnClickQuestInFamilyQuestsList;", "questsAdapter", "Lcom/blackhub/bronline/game/gui/familysystem/adapters/FamilyQuestsAdapter;", "questsList", "", "thisQuestsId", "timesOldClickOnButtonStart", "", "timesOldClickOnQuest", "clearQuestsCheckStatus", "getView", "Landroid/view/View;", "onCreateView", "inflater", "Landroid/view/LayoutInflater;", TtmlNode.RUBY_CONTAINER, "Landroid/view/ViewGroup;", "onLayoutClose", "onLayoutShown", "setDataInView", "setInfoAboutQuest", "setLogicForQuestsClick", "setNullableParameters", "setStartParameters", "app_siteRelease"}, k = 1, mv = {1, 9, 0}, xi = 48)
/* loaded from: classes3.dex */
public final class UIFamilyQuests extends UILayout {
    public static final int $stable = 8;

    @Nullable
    public final ActionsWithJSON actionsWithJSON;

    @Nullable
    public Animation anim;
    public FamiliesQuestsLayoutBinding binding;

    @Nullable
    public final JNIActivity mainActivity;

    @NotNull
    public final GUIFamilySystem mainRoot;

    @Nullable
    public Function2<? super FamilySystemTasksObj, ? super Integer, Unit> onClickQuestInFamilyQuestsList;

    @Nullable
    public FamilyQuestsAdapter questsAdapter;

    @NotNull
    public final List<FamilySystemTasksObj> questsList;
    public int thisQuestsId;
    public long timesOldClickOnButtonStart;
    public long timesOldClickOnQuest;

    @Override // com.blackhub.bronline.game.common.UILayout
    public void onLayoutClose() {
    }

    @Override // com.blackhub.bronline.game.common.UILayout
    public void onLayoutShown() {
    }

    public UIFamilyQuests(@NotNull GUIFamilySystem mainRoot, @Nullable JNIActivity jNIActivity, @Nullable ActionsWithJSON actionsWithJSON) {
        Intrinsics.checkNotNullParameter(mainRoot, "mainRoot");
        this.mainRoot = mainRoot;
        this.mainActivity = jNIActivity;
        this.actionsWithJSON = actionsWithJSON;
        this.questsList = new ArrayList();
        this.thisQuestsId = -1;
    }

    public final void setStartParameters() {
        if (this.questsList.size() > 0) {
            clearQuestsCheckStatus();
            FamilySystemTasksObj familySystemTasksObj = this.questsList.get(0);
            setInfoAboutQuest(familySystemTasksObj);
            this.thisQuestsId = familySystemTasksObj.getId();
        }
        setDataInView();
    }

    public final void clearQuestsCheckStatus() {
        int size = this.questsList.size();
        int i = 0;
        while (i < size) {
            this.questsList.get(i).setClicked(i == 0);
            i++;
        }
    }

    @Override // com.blackhub.bronline.game.common.UILayout
    @NotNull
    /* renamed from: getView */
    public View getMViewRoot() {
        FamiliesQuestsLayoutBinding familiesQuestsLayoutBinding = this.binding;
        if (familiesQuestsLayoutBinding == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
            familiesQuestsLayoutBinding = null;
        }
        ConstraintLayout root = familiesQuestsLayoutBinding.getRoot();
        Intrinsics.checkNotNullExpressionValue(root, "getRoot(...)");
        return root;
    }

    @Override // com.blackhub.bronline.game.common.UILayout
    @NotNull
    public View onCreateView(@NotNull LayoutInflater inflater, @Nullable ViewGroup r4) {
        Intrinsics.checkNotNullParameter(inflater, "inflater");
        FamiliesQuestsLayoutBinding inflate = FamiliesQuestsLayoutBinding.inflate(inflater);
        Intrinsics.checkNotNullExpressionValue(inflate, "inflate(...)");
        this.binding = inflate;
        FamiliesQuestsLayoutBinding familiesQuestsLayoutBinding = null;
        if (inflate == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
            inflate = null;
        }
        this.anim = AnimationUtils.loadAnimation(inflate.getRoot().getContext(), R.anim.button_click);
        List<FamilySystemTasksObj> list = this.questsList;
        list.clear();
        FamilySystemList familySystemListFromJson = this.mainRoot.getFamilySystemListFromJson();
        if (familySystemListFromJson != null) {
            list.addAll(familySystemListFromJson.getTasksList());
        }
        setLogicForQuestsClick();
        FamiliesQuestsLayoutBinding familiesQuestsLayoutBinding2 = this.binding;
        if (familiesQuestsLayoutBinding2 == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
            familiesQuestsLayoutBinding2 = null;
        }
        familiesQuestsLayoutBinding2.buttonStartQuest.setOnClickListener(new View.OnClickListener() { // from class: com.blackhub.bronline.game.gui.familysystem.UIFamilyQuests$$ExternalSyntheticLambda0
            public /* synthetic */ UIFamilyQuests$$ExternalSyntheticLambda0() {
            }

            @Override // android.view.View.OnClickListener
            public final void onClick(View view) {
                UIFamilyQuests.onCreateView$lambda$3(UIFamilyQuests.this, view);
            }
        });
        FamiliesQuestsLayoutBinding familiesQuestsLayoutBinding3 = this.binding;
        if (familiesQuestsLayoutBinding3 == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
            familiesQuestsLayoutBinding3 = null;
        }
        familiesQuestsLayoutBinding3.valueQuestsDescription.setMovementMethod(new ScrollingMovementMethod());
        FamiliesQuestsLayoutBinding familiesQuestsLayoutBinding4 = this.binding;
        if (familiesQuestsLayoutBinding4 == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
        } else {
            familiesQuestsLayoutBinding = familiesQuestsLayoutBinding4;
        }
        ConstraintLayout root = familiesQuestsLayoutBinding.getRoot();
        Intrinsics.checkNotNullExpressionValue(root, "getRoot(...)");
        return root;
    }

    public static final void onCreateView$lambda$3(UIFamilyQuests this$0, View view) {
        Intrinsics.checkNotNullParameter(this$0, "this$0");
        if (System.currentTimeMillis() - this$0.timesOldClickOnButtonStart > 500) {
            this$0.timesOldClickOnButtonStart = System.currentTimeMillis();
            ActionsWithJSON actionsWithJSON = this$0.actionsWithJSON;
            if (actionsWithJSON != null) {
                actionsWithJSON.startQuest(this$0.thisQuestsId);
            }
        }
    }

    private final void setDataInView() {
        this.questsAdapter = new FamilyQuestsAdapter(this.questsList, this.onClickQuestInFamilyQuestsList, this.mainActivity);
        FamiliesQuestsLayoutBinding familiesQuestsLayoutBinding = this.binding;
        FamiliesQuestsLayoutBinding familiesQuestsLayoutBinding2 = null;
        if (familiesQuestsLayoutBinding == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
            familiesQuestsLayoutBinding = null;
        }
        RecyclerView recyclerView = familiesQuestsLayoutBinding.questionsList;
        FamiliesQuestsLayoutBinding familiesQuestsLayoutBinding3 = this.binding;
        if (familiesQuestsLayoutBinding3 == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
        } else {
            familiesQuestsLayoutBinding2 = familiesQuestsLayoutBinding3;
        }
        recyclerView.setLayoutManager(new LinearLayoutManager(familiesQuestsLayoutBinding2.getRoot().getContext(), 1, false));
        recyclerView.setAdapter(this.questsAdapter);
    }

    public final void setLogicForQuestsClick() {
        this.onClickQuestInFamilyQuestsList = new Function2<FamilySystemTasksObj, Integer, Unit>() { // from class: com.blackhub.bronline.game.gui.familysystem.UIFamilyQuests$setLogicForQuestsClick$1
            public UIFamilyQuests$setLogicForQuestsClick$1() {
            }

            @Override // kotlin.jvm.functions.Function2
            public /* bridge */ /* synthetic */ Unit invoke(FamilySystemTasksObj familySystemTasksObj, Integer num) {
                invoke(familySystemTasksObj, num.intValue());
                return Unit.INSTANCE;
            }

            public void invoke(@NotNull FamilySystemTasksObj thisItem, int position) {
                long j;
                FamilyQuestsAdapter familyQuestsAdapter;
                Intrinsics.checkNotNullParameter(thisItem, "thisItem");
                long currentTimeMillis = System.currentTimeMillis();
                j = UIFamilyQuests.this.timesOldClickOnQuest;
                if (currentTimeMillis - j > 300) {
                    UIFamilyQuests.this.timesOldClickOnQuest = System.currentTimeMillis();
                    thisItem.setClicked(true);
                    familyQuestsAdapter = UIFamilyQuests.this.questsAdapter;
                    if (familyQuestsAdapter != null) {
                        familyQuestsAdapter.notifyItemChanged(position);
                        familyQuestsAdapter.setCheckOnlyElement(position);
                    }
                    UIFamilyQuests.this.thisQuestsId = thisItem.getId();
                    UIFamilyQuests.this.setInfoAboutQuest(thisItem);
                }
            }
        };
    }

    public final void setInfoAboutQuest(FamilySystemTasksObj thisItem) {
        int scoreReward;
        FamiliesQuestsLayoutBinding familiesQuestsLayoutBinding = this.binding;
        if (familiesQuestsLayoutBinding == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
            familiesQuestsLayoutBinding = null;
        }
        SpannableString spannableString = new SpannableString(thisItem.getTitle());
        spannableString.setSpan(new ForegroundColorSpan(ContextCompat.getColor(getContext(), R.color.yellow)), 0, spannableString.length(), 33);
        TextView textView = familiesQuestsLayoutBinding.questsTitle;
        JNIActivity jNIActivity = this.mainActivity;
        textView.setText(jNIActivity != null ? jNIActivity.getString(R.string.families_questions_title, spannableString) : null);
        familiesQuestsLayoutBinding.valueQuestsDescription.setText(thisItem.getDescription());
        TextView textView2 = familiesQuestsLayoutBinding.present1Value;
        JNIActivity jNIActivity2 = this.mainActivity;
        textView2.setText(jNIActivity2 != null ? jNIActivity2.getString(R.string.common_string_with_ruble, String.valueOf(thisItem.getMoneyReward())) : null);
        if (thisItem.getScoreReward() < 0) {
            scoreReward = thisItem.getScoreReward() * (-1);
            familiesQuestsLayoutBinding.present2Item.setImageResource(R.drawable.ic_gold_down);
        } else {
            scoreReward = thisItem.getScoreReward();
            familiesQuestsLayoutBinding.present2Item.setImageResource(R.drawable.ic_gold_up);
        }
        TextView textView3 = familiesQuestsLayoutBinding.present2Value;
        JNIActivity jNIActivity3 = this.mainActivity;
        textView3.setText(jNIActivity3 != null ? jNIActivity3.getString(R.string.families_quests_present_2, String.valueOf(scoreReward)) : null);
        TextView textView4 = familiesQuestsLayoutBinding.present3Value;
        JNIActivity jNIActivity4 = this.mainActivity;
        textView4.setText(jNIActivity4 != null ? jNIActivity4.getString(R.string.families_quests_present_3, String.valueOf(thisItem.getTokenReward())) : null);
    }

    public final void setNullableParameters() {
        this.anim = null;
        this.questsAdapter = null;
        this.onClickQuestInFamilyQuestsList = null;
        this.questsList.clear();
    }
}
