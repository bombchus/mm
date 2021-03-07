#include <ultra64.h>
#include <global.h>

ActorCutscene actorCutscenesGlobalCutscenes[8] = {
    { 0xFF9C, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFF, 0xFF, 0xFFFF, 0xFF, 0xFF },
    { 0xFF9C, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFF, 0xFF, 0xFFFF, 0xFF, 0xFF },
    { 0xFF9C, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFF, 0xFF, 0xFFFF, 0xFF, 0xFF },
    { 0x0002, 0xFFFF, 0xFFE7, 0xFFFF, 0xFFFF, 0xFF, 0xFF, 0x0000, 0x00, 0x20 },
    { 0x7FFD, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFF, 0xFF, 0xFFFF, 0x00, 0xFF },
    { 0x7FFC, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFF, 0xFF, 0xFFFF, 0x00, 0xFF },
    { 0x7FFE, 0xFFFE, 0xFFF2, 0xFFFF, 0xFFFF, 0x00, 0xFF, 0xFFFF, 0x00, 0x20 },
    { 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x00, 0xFF, 0xFFFF, 0x00, 0x20 },
};

ActorCutsceneManager gActorCSMgr = {
    -1, 0, -1, 0, NULL, 0, NULL, 0, 0,
};

s16 func_800F1460(s16 param_1) {
    u16 alpha;

    switch (param_1) {
        case 0:
            alpha = 2;
            break;
        case 1:
            alpha = 0x32;
            break;
        case 2:
            alpha = 5;
            break;
        case 3:
            alpha = 0xE;
            break;
        case 4:
            alpha = 0xF;
            break;
        case 5:
            alpha = 0x10;
            break;
        case 6:
            alpha = 0x11;
            break;
        case 7:
            alpha = 4;
            break;
        default:
            alpha = 0x32;
            break;
    }
    Interface_ChangeAlpha(alpha);

    return alpha;
}

ActorCutscene* ActorCutscene_GetCutsceneImpl(s16 index) {
    if (index < 120) {
        return &actorCutscenes[index];
    } else {
        index -= 120;
        return &actorCutscenesGlobalCutscenes[index];
    }
}

void ActorCutscene_Init(GlobalContext* ctxt, ActorCutscene* cutscenes, s16 num) {
    s32 i;

    actorCutscenes = cutscenes;
    actorCutsceneCount = num;

    for (i = 0; i < ARRAY_COUNT(actorCutsceneWaiting); i++) {
        actorCutsceneWaiting[i] = 0;
        actorCutsceneNextCutscenes[i] = 0;
    }

    gActorCSMgr.actorCutsceneEnding = -1;
    gActorCSMgr.actorCutscenesGlobalCtxt = ctxt;
    gActorCSMgr.actorCutsceneCurrentLength = -1;
    gActorCSMgr.actorCutsceneCurrentCutsceneActor = NULL;
    gActorCSMgr.actorCutsceneCurrentCamera = 0;
    gActorCSMgr.unk16 = 0;
    gActorCSMgr.actorCutsceneCurrent = gActorCSMgr.actorCutsceneEnding;
}

void func_800F15D8(Camera* camera) {
    if (camera != NULL) {
        memcpy(&gActorCSMgr.actorCutscenesGlobalCtxt->activeCameras[3], camera, sizeof(Camera));
        gActorCSMgr.actorCutscenesGlobalCtxt->activeCameras[3].unk164 = camera->unk164;
        func_800DE308(&gActorCSMgr.actorCutscenesGlobalCtxt->activeCameras[3], 0x100);
        gActorCSMgr.unk16 = 1;
    }
}

void ActorCutscene_ClearWaiting(void) {
    s32 i;

    for (i = 0; i < ARRAY_COUNT(actorCutsceneWaiting); i++) {
        actorCutsceneWaiting[i] = 0;
    }
}

void ActorCutscene_ClearNextCutscenes(void) {
    s32 i;

    for (i = 0; i < ARRAY_COUNT(actorCutsceneNextCutscenes); i++) {
        actorCutsceneNextCutscenes[i] = 0;
    }
}

#ifdef NON_MATCHING
// Single bne args swap, ordering differences in loop variable increments
s16 ActorCutscene_MarkNextCutscenes(void) {
    s16 phi_s1;
    s32 phi_s3;
    s32 phi_s2;
    s16 phi_s4;
    s16 phi_fp;
    s32 phi_s5;

    phi_s5 = 0;
    phi_fp = -1;
    phi_s4 = SHT_MAX;
    for (phi_s3 = 0; phi_s3 != 0x10; phi_s3++) {
        for (phi_s1 = 1, phi_s2 = 0; phi_s2 != 8; phi_s1 <<= 1, phi_s2++) {
            if (actorCutsceneWaiting[phi_s3] & phi_s1) {
                s16 temp_s0 = (phi_s3 << 3) | phi_s2;
                s16 temp_v1 = ActorCutscene_GetCutsceneImpl(temp_s0)->priority;

                if (temp_v1 == -1) {
                    actorCutsceneNextCutscenes[phi_s3] |= phi_s1;
                } else if (temp_v1 < phi_s4 && temp_v1 > 0) {
                    phi_fp = temp_s0;
                    phi_s4 = temp_v1;
                }
                phi_s5++;
            }
        }
    }
    if (phi_fp != -1) {
        actorCutsceneNextCutscenes[phi_fp >> 3] |= 1 << (phi_fp & 7);
    }
    return phi_s5;
}
#else
#pragma GLOBAL_ASM("asm/non_matchings/z_eventmgr/ActorCutscene_MarkNextCutscenes.asm")
#endif

void ActorCutscene_End(void) {
    ActorCutscene* actorCs;
    s16 old164;
    s16 old14C;

    switch (gActorCSMgr.actorCutsceneStartMethod) {
        case 2:
            gActorCSMgr.actorCutsceneCurrentCutsceneActor->flags &= 0xFFEFFFFF;
        case 1:
            func_800B7298(gActorCSMgr.actorCutscenesGlobalCtxt, 0, 6);
            gActorCSMgr.actorCutsceneStartMethod = 0;
            break;
    }

    actorCs = ActorCutscene_GetCutsceneImpl(gActorCSMgr.actorCutsceneCurrent);

    switch (actorCs->sound) {
        case 1:
            play_sound(0x4807U);
            break;
        case 2:
            play_sound(0x4802U);
            break;
        default:
            break;
    }

#define RET_CAM gActorCSMgr.actorCutscenesGlobalCtxt->cameraPtrs[gActorCSMgr.actorCutsceneReturnCamera]
#define CUR_CAM gActorCSMgr.actorCutscenesGlobalCtxt->cameraPtrs[gActorCSMgr.actorCutsceneCurrentCamera]

    switch (actorCs->unkE) {
        case 2:
            func_801699D4(gActorCSMgr.actorCutscenesGlobalCtxt, gActorCSMgr.actorCutsceneReturnCamera,
                          gActorCSMgr.actorCutsceneCurrentCamera);
            RET_CAM->unk14C = (RET_CAM->unk14C & ~0x100) | (CUR_CAM->unk14C & 0x100);
            ActorCutscene_SetIntentToPlay(0x7E);
            break;
        case 0:
        default:
            func_801699D4(gActorCSMgr.actorCutscenesGlobalCtxt, gActorCSMgr.actorCutsceneReturnCamera,
                          gActorCSMgr.actorCutsceneCurrentCamera);
            RET_CAM->unk14C = (RET_CAM->unk14C & ~0x100) | (CUR_CAM->unk14C & 0x100);
            break;
        case 1:
            old164 = RET_CAM->unk164;
            old14C = RET_CAM->unk14C;

            if (gActorCSMgr.unk16 != 0) {
                memcpy(RET_CAM, &gActorCSMgr.actorCutscenesGlobalCtxt->activeCameras[3], sizeof(Camera));

                RET_CAM->unk14C = (RET_CAM->unk14C & ~0x100) | (CUR_CAM->unk14C & 0x100);

                RET_CAM->unk14C = (RET_CAM->unk14C & ~4) | (old14C & 4);
                gActorCSMgr.unk16 = 0;
            }
            RET_CAM->unk164 = old164;
            break;
    }

    if (gActorCSMgr.actorCutsceneCurrentCamera != NULL) {
        func_80169600(gActorCSMgr.actorCutscenesGlobalCtxt, gActorCSMgr.actorCutsceneCurrentCamera);
        func_80169590(gActorCSMgr.actorCutscenesGlobalCtxt, gActorCSMgr.actorCutsceneReturnCamera, 7);
    }

    gActorCSMgr.actorCutsceneCurrent = -1;
    gActorCSMgr.actorCutsceneEnding = -1;
    gActorCSMgr.actorCutsceneCurrentLength = -1;
    gActorCSMgr.actorCutsceneCurrentCutsceneActor = NULL;
    gActorCSMgr.actorCutsceneCurrentCamera = 0;
}

s16 ActorCutscene_Update(void) {
    s16 sp1E = 0;

    if (ActorCutscene_GetCanPlayNext(0x7E) != 0) {
        ActorCutscene_StartAndSetUnkLinkFields(0x7E, gActorCSMgr.actorCutscenesGlobalCtxt->actorCtx.actorList[2].first);
    }
    if (gActorCSMgr.actorCutsceneEnding == -1) {
        if (gActorCSMgr.actorCutsceneCurrent != -1) {
            if (gActorCSMgr.actorCutsceneCurrentLength > 0) {
                gActorCSMgr.actorCutsceneCurrentLength--;
            }
            sp1E = 1;
            if (gActorCSMgr.actorCutsceneCurrentLength == 0) {
                ActorCutscene_Stop(gActorCSMgr.actorCutsceneCurrent);
            }
        }
    }
    if (gActorCSMgr.actorCutsceneEnding != -1) {
        ActorCutscene_End();
        sp1E = 2;
    }
    ActorCutscene_ClearNextCutscenes();
    if (gActorCSMgr.actorCutsceneCurrent == -1) {
        if (ActorCutscene_MarkNextCutscenes() == 0 && sp1E != 0) {
            ShrinkWindow_SetLetterboxTarget(0);
        } else if (sp1E == 0) {
            func_800F15D8(Play_GetCamera(gActorCSMgr.actorCutscenesGlobalCtxt, gActorCSMgr.actorCutsceneReturnCamera));
        }
    }
    return sp1E;
}

void ActorCutscene_SetIntentToPlay(s16 index) {
    if (index >= 0) {
        actorCutsceneWaiting[index >> 3] |= 1 << (index & 7);
    }
}

s16 ActorCutscene_GetCanPlayNext(s16 index) {
    if (index == 0x7F) {
        if (gActorCSMgr.actorCutsceneCurrent == -1) {
            return 0x7F;
        } else {
            return 0;
        }
    }
    if (index < 0) {
        return -1;
    }
    return (actorCutsceneNextCutscenes[index >> 3] & (1 << (index & 7))) ? 1 : 0;
}

s16 ActorCutscene_StartAndSetUnkLinkFields(s16 index, Actor* actor) {
    s16 sp1E = ActorCutscene_Start(index, actor);

    if (sp1E >= 0) {
        func_800B7298(gActorCSMgr.actorCutscenesGlobalCtxt, 0, 7);
        if (gActorCSMgr.actorCutsceneCurrentLength == 0) {
            ActorCutscene_Stop(gActorCSMgr.actorCutsceneCurrent);
        }
        gActorCSMgr.actorCutsceneStartMethod = 1;
    }
    return sp1E;
}

s16 ActorCutscene_StartAndSetFlag(s16 index, Actor* actor) {
    s16 sp1E = ActorCutscene_Start(index, actor);

    if (sp1E >= 0) {
        func_800B7298(gActorCSMgr.actorCutscenesGlobalCtxt, 0, 7);
        if (gActorCSMgr.actorCutsceneCurrentLength == 0) {
            ActorCutscene_Stop(gActorCSMgr.actorCutsceneCurrent);
        }
        if (actor != NULL) {
            actor->flags |= 0x100000;
            gActorCSMgr.actorCutsceneStartMethod = 2;
        } else {
            gActorCSMgr.actorCutsceneStartMethod = 1;
        }
    }
    return sp1E;
}

s16 ActorCutscene_Start(s16 index, Actor* actor) {
    ActorCutscene* actorCs;
    Camera* sp28;
    Camera* temp_v0_2;
    s32 sp20;

    sp20 = 0;
    if (index < 0 || gActorCSMgr.actorCutsceneCurrent != -1) {
        return index;
    }

    gActorCSMgr.actorCutsceneStartMethod = NULL;
    actorCs = ActorCutscene_GetCutsceneImpl(index);

    ShrinkWindow_SetLetterboxTarget(actorCs->letterboxSize);
    func_800F1460(actorCs->unkC);

    if (index == 0x7F) {
        sp20 = 1;
    } else if (actorCs->unk6 != -1) {
        sp20 = 1;
    } else if (index != 0x7D && index != 0x7C) {
        sp20 = 2;
    }

    if (sp20 != 0) {
        gActorCSMgr.actorCutsceneReturnCamera = Play_GetActiveCameraIndex(gActorCSMgr.actorCutscenesGlobalCtxt);
        gActorCSMgr.actorCutsceneCurrentCamera = func_801694DC(gActorCSMgr.actorCutscenesGlobalCtxt);
        sp28 = Play_GetCamera(gActorCSMgr.actorCutscenesGlobalCtxt, gActorCSMgr.actorCutsceneCurrentCamera);
        temp_v0_2 = Play_GetCamera(gActorCSMgr.actorCutscenesGlobalCtxt, gActorCSMgr.actorCutsceneReturnCamera);
        if (temp_v0_2->state == 0x2C || temp_v0_2->state == 0x2D || temp_v0_2->state == 0x56) {
            if (func_800F21CC() != index) {
                func_800E0348(temp_v0_2);
            } else {
                Camera_ClearFlags(temp_v0_2, 4);
            }
        }
        memcpy(sp28, temp_v0_2, sizeof(Camera));
        sp28->unk164 = gActorCSMgr.actorCutsceneCurrentCamera;
        Camera_ClearFlags(sp28, 0x41);
        func_80169590(gActorCSMgr.actorCutscenesGlobalCtxt, gActorCSMgr.actorCutsceneReturnCamera, 1);
        func_80169590(gActorCSMgr.actorCutscenesGlobalCtxt, gActorCSMgr.actorCutsceneCurrentCamera, 7);

        sp28->unkA8 = gActorCSMgr.actorCutsceneCurrentCutsceneActor = actor;
        sp28->unk14A = 0;

        if (sp20 == 1) {
            func_800DFAC8(sp28, 0xA);
            func_800EDDCC(gActorCSMgr.actorCutscenesGlobalCtxt, actorCs->unk6);
            gActorCSMgr.actorCutsceneCurrentLength = actorCs->length;
        } else {
            if (actorCs->unk4 != -1) {
                func_800DFB14(sp28, actorCs->unk4);
            } else {
                func_800DFAC8(sp28, 0xA);
            }
            gActorCSMgr.actorCutsceneCurrentLength = actorCs->length;
        }
    }
    gActorCSMgr.actorCutsceneCurrent = index;

    return index;
}

s16 ActorCutscene_Stop(s16 index) {
    ActorCutscene* actorCs;

    if (index < 0) {
        return index;
    }

    actorCs = ActorCutscene_GetCutsceneImpl(gActorCSMgr.actorCutsceneCurrent);
    if (gActorCSMgr.actorCutsceneCurrentLength > 0 && actorCs->unk6 == -1) {
        return -2;
    }
    if (index != 0x7F && actorCs->unk6 != -1) {
        return -3;
    }

    if (index == 0x7F) {
        index = gActorCSMgr.actorCutsceneCurrent;
    }
    if (index == gActorCSMgr.actorCutsceneCurrent) {
        gActorCSMgr.actorCutsceneEnding = gActorCSMgr.actorCutsceneCurrent;
        return gActorCSMgr.actorCutsceneEnding;
    }
    return -1;
}

s16 ActorCutscene_GetCurrentIndex(void) {
    return gActorCSMgr.actorCutsceneCurrent;
}

ActorCutscene* ActorCutscene_GetCutscene(s16 index) {
    return ActorCutscene_GetCutsceneImpl(index);
}

s16 ActorCutscene_GetAdditionalCutscene(s16 index) {
    if (index < 0) {
        return -1;
    }
    return ActorCutscene_GetCutsceneImpl(index)->additionalCutscene;
}

s16 ActorCutscene_GetLength(s16 index) {
    if (index < 0) {
        return -1;
    }
    return ActorCutscene_GetCutsceneImpl(index)->length;
}

s16 func_800F2138(s16 index) {
    if (index < 0) {
        return -1;
    }
    return ActorCutscene_GetCutsceneImpl(index)->unk6;
}

s16 func_800F2178(s16 index) {
    if (index < 0) {
        return -1;
    }
    return ActorCutscene_GetCutsceneImpl(index)->unkB;
}

s16 ActorCutscene_GetCurrentCamera(UNK_PTR a0) {
    return gActorCSMgr.actorCutsceneCurrentCamera;
}

#ifdef NON_MATCHING
// Regalloc differences (a registers 1 off)
s16 func_800F21CC(void) {
    s32 phi_v1;

    for (phi_v1 = 0; phi_v1 < actorCutsceneCount; phi_v1++) {
        if (actorCutscenes) {}

        if (actorCutscenes[phi_v1].unk6 != -1 &&
            actorCutscenes[phi_v1].unk6 < gActorCSMgr.actorCutscenesGlobalCtxt->csCtx.cutsceneCount &&
            gActorCSMgr.actorCutscenesGlobalCtxt->curSpawn ==
                gActorCSMgr.actorCutscenesGlobalCtxt->cutsceneList[actorCutscenes[phi_v1].unk6].unk6) {
            return phi_v1;
        }
    }

    for (phi_v1 = 0; phi_v1 < actorCutsceneCount; phi_v1++) {
        if (actorCutscenes[phi_v1].unkB >= 100 &&
            actorCutscenes[phi_v1].unkB == (gActorCSMgr.actorCutscenesGlobalCtxt->curSpawn + 100)) {
            return phi_v1;
        }
    }

    return -1;
}
#else
#pragma GLOBAL_ASM("asm/non_matchings/z_eventmgr/func_800F21CC.asm")
#endif

s32 func_800F22C4(s16 param_1, Actor* actor) {
    f32 dist;
    s16 sp2A;
    s16 sp28;

    if (gActorCSMgr.actorCutsceneCurrent == -1 || param_1 == -1) {
        return 4;
    }

    func_800B8898(gActorCSMgr.actorCutscenesGlobalCtxt, actor, &sp2A, &sp28);

    dist = CamMath_Distance(
        &actor->focus.pos,
        &Play_GetCamera(gActorCSMgr.actorCutscenesGlobalCtxt, gActorCSMgr.actorCutsceneCurrentCamera)->eye);

    if (sp2A > 0x28 && sp2A < 0x118 && sp28 > 0x28 && sp28 < 0xC8 && dist < 700.0f) {
        return 1;
    }
    if (gActorCSMgr.actorCutsceneCurrentLength < 6) {
        return 2;
    }
    if (param_1 == gActorCSMgr.actorCutsceneCurrent) {
        return 0;
    }
    return 3;
}

void ActorCutscene_SetReturnCamera(s16 index) {
    gActorCSMgr.actorCutsceneReturnCamera = index;
}
