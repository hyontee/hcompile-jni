package com.blackhub.bronline.game.gui.familysystem.adapters;

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import androidx.compose.runtime.internal.StabilityInferred;
import androidx.core.content.res.ResourcesCompat;
import androidx.recyclerview.widget.RecyclerView;
import com.blackhub.bronline.R;
import com.blackhub.bronline.databinding.FamiliesQuestItemBinding;
import com.blackhub.bronline.game.core.JNIActivity;
import com.blackhub.bronline.game.gui.familysystem.adapters.FamilyQuestsAdapter;
import com.blackhub.bronline.game.gui.familysystem.data.FamilySystemTasksObj;
import java.util.List;
import kotlin.Metadata;
import kotlin.Unit;
import kotlin.jvm.functions.Function2;
import kotlin.jvm.internal.Intrinsics;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

/* JADX WARN: Classes with same name are omitted:
  C:\javanew\classes3.dex
 */
/* compiled from: FamilyQuestsAdapter.kt */
@StabilityInferred(parameters = 0)
@Metadata(d1 = {"\u0000D\n\u0002\u0018\u0002\n\u0002\u0018\u0002\n\u0002\u0018\u0002\n\u0000\n\u0002\u0010 \n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0002\u0018\u0002\n\u0002\b\u0002\n\u0002\u0010\b\n\u0000\n\u0002\u0010\u0002\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0002\b\u0006\n\u0002\u0018\u0002\n\u0002\b\u0005\b\u0007\u0018\u00002\f\u0012\b\u0012\u00060\u0002R\u00020\u00000\u0001:\u0001\u001bB]\u0012\f\u0010\u0003\u001a\b\u0012\u0004\u0012\u00020\u00050\u0004\u0012>\u0010\u0006\u001a:\u0012\u0013\u0012\u00110\u0005¢\u0006\f\b\b\u0012\b\b\t\u0012\u0004\b\b(\n\u0012\u0013\u0012\u00110\u000b¢\u0006\f\b\b\u0012\b\b\t\u0012\u0004\b\b(\f\u0012\u0004\u0012\u00020\r\u0018\u00010\u0007j\u0004\u0018\u0001`\u000e\u0012\b\u0010\u000f\u001a\u0004\u0018\u00010\u0010¢\u0006\u0002\u0010\u0011J\b\u0010\u0012\u001a\u00020\u000bH\u0016J\u001c\u0010\u0013\u001a\u00020\r2\n\u0010\u0014\u001a\u00060\u0002R\u00020\u00002\u0006\u0010\f\u001a\u00020\u000bH\u0016J\u001c\u0010\u0015\u001a\u00060\u0002R\u00020\u00002\u0006\u0010\u0016\u001a\u00020\u00172\u0006\u0010\u0018\u001a\u00020\u000bH\u0016J\u000e\u0010\u0019\u001a\u00020\r2\u0006\u0010\u001a\u001a\u00020\u000bR\u0014\u0010\u0003\u001a\b\u0012\u0004\u0012\u00020\u00050\u0004X\u0082\u0004¢\u0006\u0002\n\u0000R\u0010\u0010\u000f\u001a\u0004\u0018\u00010\u0010X\u0082\u0004¢\u0006\u0002\n\u0000RF\u0010\u0006\u001a:\u0012\u0013\u0012\u00110\u0005¢\u0006\f\b\b\u0012\b\b\t\u0012\u0004\b\b(\n\u0012\u0013\u0012\u00110\u000b¢\u0006\f\b\b\u0012\b\b\t\u0012\u0004\b\b(\f\u0012\u0004\u0012\u00020\r\u0018\u00010\u0007j\u0004\u0018\u0001`\u000eX\u0082\u0004¢\u0006\u0002\n\u0000¨\u0006\u001c"}, d2 = {"Lcom/blackhub/bronline/game/gui/familysystem/adapters/FamilyQuestsAdapter;", "Landroidx/recyclerview/widget/RecyclerView$Adapter;", "Lcom/blackhub/bronline/game/gui/familysystem/adapters/FamilyQuestsAdapter$ViewHolder;", "familyQuests", "", "Lcom/blackhub/bronline/game/gui/familysystem/data/FamilySystemTasksObj;", "onClickQuestInFamilyQuestsList", "Lkotlin/Function2;", "Lkotlin/ParameterName;", "name", "thisItem", "", "position", "", "Lcom/blackhub/bronline/game/gui/familysystem/OnClickQuestInFamilyQuestsList;", "mainActivity", "Lcom/blackhub/bronline/game/core/JNIActivity;", "(Ljava/util/List;Lkotlin/jvm/functions/Function2;Lcom/blackhub/bronline/game/core/JNIActivity;)V", "getItemCount", "onBindViewHolder", "holder", "onCreateViewHolder", "parent", "Landroid/view/ViewGroup;", "viewType", "setCheckOnlyElement", "checkedPosition", "ViewHolder", "app_siteRelease"}, k = 1, mv = {1, 9, 0}, xi = 48)
/* loaded from: classes3.dex */
public final class FamilyQuestsAdapter extends RecyclerView.Adapter<ViewHolder> {
    public static final int $stable = 8;

    @NotNull
    public final List<FamilySystemTasksObj> familyQuests;

    @Nullable
    public final JNIActivity mainActivity;

    @Nullable
    public final Function2<FamilySystemTasksObj, Integer, Unit> onClickQuestInFamilyQuestsList;

    /* JADX WARN: Multi-variable type inference failed */
    public FamilyQuestsAdapter(@NotNull List<FamilySystemTasksObj> familyQuests, @Nullable Function2<? super FamilySystemTasksObj, ? super Integer, Unit> function2, @Nullable JNIActivity jNIActivity) {
        Intrinsics.checkNotNullParameter(familyQuests, "familyQuests");
        this.familyQuests = familyQuests;
        this.onClickQuestInFamilyQuestsList = function2;
        this.mainActivity = jNIActivity;
    }

    @Override // androidx.recyclerview.widget.RecyclerView.Adapter
    @NotNull
    public ViewHolder onCreateViewHolder(@NotNull ViewGroup parent, int viewType) {
        Intrinsics.checkNotNullParameter(parent, "parent");
        FamiliesQuestItemBinding inflate = FamiliesQuestItemBinding.inflate(LayoutInflater.from(parent.getContext()), parent, false);
        Intrinsics.checkNotNullExpressionValue(inflate, "inflate(...)");
        return new ViewHolder(this, inflate);
    }

    @Override // androidx.recyclerview.widget.RecyclerView.Adapter
    public void onBindViewHolder(@NotNull ViewHolder holder, int position) {
        Intrinsics.checkNotNullParameter(holder, "holder");
        holder.bind(this.familyQuests.get(position), this.onClickQuestInFamilyQuestsList);
    }

    @Override // androidx.recyclerview.widget.RecyclerView.Adapter
    public int getItemCount() {
        return this.familyQuests.size();
    }

    /* JADX WARN: Classes with same name are omitted:
  C:\javanew\classes3.dex
 */
    /* compiled from: FamilyQuestsAdapter.kt */
    @Metadata(d1 = {"\u00006\n\u0002\u0018\u0002\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0002\b\u0002\n\u0002\u0010\u0002\n\u0000\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0002\u0018\u0002\n\u0002\b\u0002\n\u0002\u0010\b\n\u0000\n\u0002\u0018\u0002\n\u0000\b\u0086\u0004\u0018\u00002\u00020\u0001B\r\u0012\u0006\u0010\u0002\u001a\u00020\u0003¢\u0006\u0002\u0010\u0004JN\u0010\u0005\u001a\u00020\u00062\u0006\u0010\u0007\u001a\u00020\b2>\u0010\t\u001a:\u0012\u0013\u0012\u00110\b¢\u0006\f\b\u000b\u0012\b\b\f\u0012\u0004\b\b(\r\u0012\u0013\u0012\u00110\u000e¢\u0006\f\b\u000b\u0012\b\b\f\u0012\u0004\b\b(\u000f\u0012\u0004\u0012\u00020\u0006\u0018\u00010\nj\u0004\u0018\u0001`\u0010R\u000e\u0010\u0002\u001a\u00020\u0003X\u0082\u0004¢\u0006\u0002\n\u0000¨\u0006\u0011"}, d2 = {"Lcom/blackhub/bronline/game/gui/familysystem/adapters/FamilyQuestsAdapter$ViewHolder;", "Landroidx/recyclerview/widget/RecyclerView$ViewHolder;", "binding", "Lcom/blackhub/bronline/databinding/FamiliesQuestItemBinding;", "(Lcom/blackhub/bronline/game/gui/familysystem/adapters/FamilyQuestsAdapter;Lcom/blackhub/bronline/databinding/FamiliesQuestItemBinding;)V", "bind", "", "thisQuest", "Lcom/blackhub/bronline/game/gui/familysystem/data/FamilySystemTasksObj;", "onClickQuestInFamilyQuestsList", "Lkotlin/Function2;", "Lkotlin/ParameterName;", "name", "thisItem", "", "position", "Lcom/blackhub/bronline/game/gui/familysystem/OnClickQuestInFamilyQuestsList;", "app_siteRelease"}, k = 1, mv = {1, 9, 0}, xi = 48)
    public final class ViewHolder extends RecyclerView.ViewHolder {

        @NotNull
        public final FamiliesQuestItemBinding binding;
        public final /* synthetic */ FamilyQuestsAdapter this$0;

        /* JADX WARN: 'super' call moved to the top of the method (can break code semantics) */
        public ViewHolder(@NotNull FamilyQuestsAdapter familyQuestsAdapter, FamiliesQuestItemBinding binding) {
            super(binding.getRoot());
            Intrinsics.checkNotNullParameter(binding, "binding");
            this.this$0 = familyQuestsAdapter;
            this.binding = binding;
        }

        public final void bind(@NotNull final FamilySystemTasksObj thisQuest, @Nullable final Function2<? super FamilySystemTasksObj, ? super Integer, Unit> onClickQuestInFamilyQuestsList) {
            Intrinsics.checkNotNullParameter(thisQuest, "thisQuest");
            FamiliesQuestItemBinding familiesQuestItemBinding = this.binding;
            JNIActivity jNIActivity = this.this$0.mainActivity;
            if (jNIActivity != null) {
                if (thisQuest.isClicked()) {
                    familiesQuestItemBinding.getRoot().setBackground(ResourcesCompat.getDrawable(jNIActivity.getResources(), R.drawable.bg_rectangle_blue_cr5, null));
                } else {
                    familiesQuestItemBinding.getRoot().setBackground(ResourcesCompat.getDrawable(jNIActivity.getResources(), R.drawable.bg_rectangle_white15_cr5, null));
                }
            }
            familiesQuestItemBinding.questionsTitle.setText(thisQuest.getTitle());
            if (thisQuest.getScoreReward() >= 0) {
                familiesQuestItemBinding.present2Item.setImageResource(R.drawable.ic_gold_up);
            } else {
                familiesQuestItemBinding.present2Item.setImageResource(R.drawable.ic_gold_down);
            }
            familiesQuestItemBinding.getRoot().setOnClickListener(new View.OnClickListener() { // from class: com.blackhub.bronline.game.gui.familysystem.adapters.FamilyQuestsAdapter$ViewHolder$$ExternalSyntheticLambda0
                @Override // android.view.View.OnClickListener
                public final void onClick(View view) {
                    FamilyQuestsAdapter.ViewHolder.bind$lambda$2$lambda$1(Function2.this, thisQuest, this, view);
                }
            });
        }

        public static final void bind$lambda$2$lambda$1(Function2 function2, FamilySystemTasksObj thisQuest, ViewHolder this$0, View view) {
            Intrinsics.checkNotNullParameter(thisQuest, "$thisQuest");
            Intrinsics.checkNotNullParameter(this$0, "this$0");
            if (function2 != null) {
                function2.invoke(thisQuest, Integer.valueOf(this$0.getLayoutPosition()));
            }
        }
    }

    public final void setCheckOnlyElement(int checkedPosition) {
        int size = this.familyQuests.size();
        for (int i = 0; i < size; i++) {
            if (this.familyQuests.get(i).isClicked() && i != checkedPosition) {
                this.familyQuests.get(i).setClicked(false);
                notifyItemChanged(i);
            }
        }
    }
}
