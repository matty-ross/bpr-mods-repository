#pragma once


#include <array>


struct ChallengeID
{
    uint64_t Number;
    char String[24];
};


inline constexpr ChallengeID k_FallbackChallengeID =
{
    .Number = 0x000000000008DACC,
    .String = "580300",
};


inline constexpr std::array<ChallengeID, 500> k_VanillaChallengeIDs =
{
    ChallengeID{ .Number = 0x000000000009262A, .String = "599594" },
    ChallengeID{ .Number = 0x0000000000092921, .String = "600353" },
    ChallengeID{ .Number = 0x0000000000092A3E, .String = "600638" },
    ChallengeID{ .Number = 0x0000000000092924, .String = "600356" },
    ChallengeID{ .Number = 0x000000000008DCDF, .String = "580831" },
    ChallengeID{ .Number = 0x0000000000092613, .String = "599571" },
    ChallengeID{ .Number = 0x00000000000928EC, .String = "600300" },
    ChallengeID{ .Number = 0x00000000000B2730, .String = "730928" },
    ChallengeID{ .Number = 0x00000000000B2761, .String = "730977" },
    ChallengeID{ .Number = 0x00000000000B28A2, .String = "731298" },
    ChallengeID{ .Number = 0x00000000000B2729, .String = "730921" },
    ChallengeID{ .Number = 0x00000000000B228E, .String = "729742" },
    ChallengeID{ .Number = 0x00000000000B2618, .String = "730648" },
    ChallengeID{ .Number = 0x00000000000B2722, .String = "730914" },
    ChallengeID{ .Number = 0x000000000008DACC, .String = "580300" },
    ChallengeID{ .Number = 0x00000000000B2A46, .String = "731718" },
    ChallengeID{ .Number = 0x0000000000092619, .String = "599577" },
    ChallengeID{ .Number = 0x00000000000B2BBB, .String = "732091" },
    ChallengeID{ .Number = 0x0000000000091CCA, .String = "597194" },
    ChallengeID{ .Number = 0x0000000000092671, .String = "599665" },
    ChallengeID{ .Number = 0x0000000000091CE3, .String = "597219" },
    ChallengeID{ .Number = 0x0000000000091969, .String = "596329" },
    ChallengeID{ .Number = 0x00000000000B294D, .String = "731469" },
    ChallengeID{ .Number = 0x00000000000B2755, .String = "730965" },
    ChallengeID{ .Number = 0x00000000000B2BE8, .String = "732136" },
    ChallengeID{ .Number = 0x0000000000091FC6, .String = "597958" },
    ChallengeID{ .Number = 0x000000000008F53C, .String = "587068" },
    ChallengeID{ .Number = 0x00000000000B2766, .String = "730982" },
    ChallengeID{ .Number = 0x0000000000092626, .String = "599590" },
    ChallengeID{ .Number = 0x000000000009269C, .String = "599708" },
    ChallengeID{ .Number = 0x00000000000B2561, .String = "730465" },
    ChallengeID{ .Number = 0x00000000000B2A25, .String = "731685" },
    ChallengeID{ .Number = 0x00000000000B2A39, .String = "731705" },
    ChallengeID{ .Number = 0x00000000000B275D, .String = "730973" },
    ChallengeID{ .Number = 0x00000000000B2A34, .String = "731700" },
    ChallengeID{ .Number = 0x00000000000B2570, .String = "730480" },
    ChallengeID{ .Number = 0x00000000000B2566, .String = "730470" },
    ChallengeID{ .Number = 0x00000000000925ED, .String = "599533" },
    ChallengeID{ .Number = 0x00000000000B2590, .String = "730512" },
    ChallengeID{ .Number = 0x0000000000092646, .String = "599622" },
    ChallengeID{ .Number = 0x00000000000925D1, .String = "599505" },
    ChallengeID{ .Number = 0x0000000000092A47, .String = "600647" },
    ChallengeID{ .Number = 0x0000000000091CB0, .String = "597168" },
    ChallengeID{ .Number = 0x000000000009264A, .String = "599626" },
    ChallengeID{ .Number = 0x00000000000B28A7, .String = "731303" },
    ChallengeID{ .Number = 0x00000000000B256B, .String = "730475" },
    ChallengeID{ .Number = 0x000000000009265A, .String = "599642" },
    ChallengeID{ .Number = 0x00000000000925F1, .String = "599537" },
    ChallengeID{ .Number = 0x00000000000B2790, .String = "731024" },
    ChallengeID{ .Number = 0x00000000000B2530, .String = "730416" },
    ChallengeID{ .Number = 0x0000000000092690, .String = "599696" },
    ChallengeID{ .Number = 0x00000000000B22E2, .String = "729826" },
    ChallengeID{ .Number = 0x00000000000B2A2F, .String = "731695" },
    ChallengeID{ .Number = 0x000000000008DC81, .String = "580737" },
    ChallengeID{ .Number = 0x000000000008F7DA, .String = "587738" },
    ChallengeID{ .Number = 0x00000000000B2910, .String = "731408" },
    ChallengeID{ .Number = 0x00000000000B291C, .String = "731420" },
    ChallengeID{ .Number = 0x000000000008EFA2, .String = "585634" },
    ChallengeID{ .Number = 0x00000000000B260C, .String = "730636" },
    ChallengeID{ .Number = 0x0000000000092694, .String = "599700" },
    ChallengeID{ .Number = 0x0000000000091FFE, .String = "598014" },
    ChallengeID{ .Number = 0x00000000000B2784, .String = "731012" },
    ChallengeID{ .Number = 0x00000000000B2583, .String = "730499" },
    ChallengeID{ .Number = 0x00000000000925FA, .String = "599546" },
    ChallengeID{ .Number = 0x00000000000B2BD4, .String = "732116" },
    ChallengeID{ .Number = 0x0000000000091BF4, .String = "596980" },
    ChallengeID{ .Number = 0x00000000000B2788, .String = "731016" },
    ChallengeID{ .Number = 0x00000000000B2A5F, .String = "731743" },
    ChallengeID{ .Number = 0x0000000000092642, .String = "599618" },
    ChallengeID{ .Number = 0x000000000008ED20, .String = "584992" },
    ChallengeID{ .Number = 0x000000000009268C, .String = "599692" },
    ChallengeID{ .Number = 0x0000000000091FC2, .String = "597954" },
    ChallengeID{ .Number = 0x00000000000B2614, .String = "730644" },
    ChallengeID{ .Number = 0x00000000000925E5, .String = "599525" },
    ChallengeID{ .Number = 0x0000000000092669, .String = "599657" },
    ChallengeID{ .Number = 0x00000000000B2759, .String = "730969" },
    ChallengeID{ .Number = 0x00000000000B273E, .String = "730942" },
    ChallengeID{ .Number = 0x00000000000B2557, .String = "730455" },
    ChallengeID{ .Number = 0x00000000000B2610, .String = "730640" },
    ChallengeID{ .Number = 0x00000000000B274D, .String = "730957" },
    ChallengeID{ .Number = 0x00000000000B257F, .String = "730495" },
    ChallengeID{ .Number = 0x000000000009263A, .String = "599610" },
    ChallengeID{ .Number = 0x000000000009265E, .String = "599646" },
    ChallengeID{ .Number = 0x00000000000B2778, .String = "731000" },
    ChallengeID{ .Number = 0x00000000000B261C, .String = "730652" },
    ChallengeID{ .Number = 0x0000000000091C06, .String = "596998" },
    ChallengeID{ .Number = 0x00000000000B2745, .String = "730949" },
    ChallengeID{ .Number = 0x00000000000B2A50, .String = "731728" },
    ChallengeID{ .Number = 0x00000000000B277F, .String = "731007" },
    ChallengeID{ .Number = 0x00000000000B22DA, .String = "729818" },
    ChallengeID{ .Number = 0x00000000000B2620, .String = "730656" },
    ChallengeID{ .Number = 0x000000000009263E, .String = "599614" },
    ChallengeID{ .Number = 0x0000000000092A13, .String = "600595" },
    ChallengeID{ .Number = 0x0000000000091CBA, .String = "597178" },
    ChallengeID{ .Number = 0x00000000000936A2, .String = "603810" },
    ChallengeID{ .Number = 0x0000000000091FBE, .String = "597950" },
    ChallengeID{ .Number = 0x000000000008DB75, .String = "580469" },
    ChallengeID{ .Number = 0x0000000000091C02, .String = "596994" },
    ChallengeID{ .Number = 0x0000000000091953, .String = "596307" },
    ChallengeID{ .Number = 0x0000000000092688, .String = "599688" },
    ChallengeID{ .Number = 0x00000000000B278C, .String = "731020" },
    ChallengeID{ .Number = 0x00000000000B2956, .String = "731478" },
    ChallengeID{ .Number = 0x00000000000B2A4B, .String = "731723" },
    ChallengeID{ .Number = 0x00000000000B2A2A, .String = "731690" },
    ChallengeID{ .Number = 0x00000000000925CD, .String = "599501" },
    ChallengeID{ .Number = 0x000000000008F0C6, .String = "585926" },
    ChallengeID{ .Number = 0x000000000009262E, .String = "599598" },
    ChallengeID{ .Number = 0x00000000000B2A20, .String = "731680" },
    ChallengeID{ .Number = 0x00000000000B2737, .String = "730935" },
    ChallengeID{ .Number = 0x00000000000B22CF, .String = "729807" },
    ChallengeID{ .Number = 0x00000000000B252C, .String = "730412" },
    ChallengeID{ .Number = 0x00000000000B255B, .String = "730459" },
    ChallengeID{ .Number = 0x00000000000B2637, .String = "730679" },
    ChallengeID{ .Number = 0x000000000009286E, .String = "600174" },
    ChallengeID{ .Number = 0x0000000000092604, .String = "599556" },
    ChallengeID{ .Number = 0x0000000000091C41, .String = "597057" },
    ChallengeID{ .Number = 0x000000000008DCCB, .String = "580811" },
    ChallengeID{ .Number = 0x000000000008DB03, .String = "580355" },
    ChallengeID{ .Number = 0x00000000000B2527, .String = "730407" },
    ChallengeID{ .Number = 0x00000000000B2587, .String = "730503" },
    ChallengeID{ .Number = 0x0000000000092A01, .String = "600577" },
    ChallengeID{ .Number = 0x00000000000B2942, .String = "731458" },
    ChallengeID{ .Number = 0x00000000000B292C, .String = "731436" },
    ChallengeID{ .Number = 0x000000000008F519, .String = "587033" },
    ChallengeID{ .Number = 0x00000000000B2A5A, .String = "731738" },
    ChallengeID{ .Number = 0x0000000000091BCC, .String = "596940" },
    ChallengeID{ .Number = 0x00000000000B2BDE, .String = "732126" },
    ChallengeID{ .Number = 0x00000000000B2595, .String = "730517" },
    ChallengeID{ .Number = 0x00000000000925E1, .String = "599521" },
    ChallengeID{ .Number = 0x00000000000927F3, .String = "600051" },
    ChallengeID{ .Number = 0x00000000000B257A, .String = "730490" },
    ChallengeID{ .Number = 0x00000000000B2553, .String = "730451" },
    ChallengeID{ .Number = 0x000000000008EE38, .String = "585272" },
    ChallengeID{ .Number = 0x00000000000B2575, .String = "730485" },
    ChallengeID{ .Number = 0x000000000008F68F, .String = "587407" },
    ChallengeID{ .Number = 0x00000000000B2A55, .String = "731733" },
    ChallengeID{ .Number = 0x0000000000092039, .String = "598073" },
    ChallengeID{ .Number = 0x000000000009481B, .String = "608283" },
    ChallengeID{ .Number = 0x00000000000925D9, .String = "599513" },
    ChallengeID{ .Number = 0x0000000000092035, .String = "598069" },
    ChallengeID{ .Number = 0x000000000008E8C3, .String = "583875" },
    ChallengeID{ .Number = 0x000000000008EE71, .String = "585329" },
    ChallengeID{ .Number = 0x000000000008DC82, .String = "580738" },
    ChallengeID{ .Number = 0x00000000000928CB, .String = "600267" },
    ChallengeID{ .Number = 0x0000000000092031, .String = "598065" },
    ChallengeID{ .Number = 0x0000000000094813, .String = "608275" },
    ChallengeID{ .Number = 0x000000000009288D, .String = "600205" },
    ChallengeID{ .Number = 0x000000000008EE93, .String = "585363" },
    ChallengeID{ .Number = 0x0000000000091C12, .String = "597010" },
    ChallengeID{ .Number = 0x0000000000096C9D, .String = "617629" },
    ChallengeID{ .Number = 0x00000000000920C6, .String = "598214" },
    ChallengeID{ .Number = 0x000000000008F21E, .String = "586270" },
    ChallengeID{ .Number = 0x000000000008EECB, .String = "585419" },
    ChallengeID{ .Number = 0x0000000000092A4A, .String = "600650" },
    ChallengeID{ .Number = 0x0000000000091FDE, .String = "597982" },
    ChallengeID{ .Number = 0x0000000000091FEA, .String = "597994" },
    ChallengeID{ .Number = 0x000000000008EC71, .String = "584817" },
    ChallengeID{ .Number = 0x0000000000092858, .String = "600152" },
    ChallengeID{ .Number = 0x0000000000092A41, .String = "600641" },
    ChallengeID{ .Number = 0x0000000000096C8C, .String = "617612" },
    ChallengeID{ .Number = 0x0000000000091CEA, .String = "597226" },
    ChallengeID{ .Number = 0x000000000009254F, .String = "599375" },
    ChallengeID{ .Number = 0x0000000000092652, .String = "599634" },
    ChallengeID{ .Number = 0x0000000000091B65, .String = "596837" },
    ChallengeID{ .Number = 0x000000000008F82A, .String = "587818" },
    ChallengeID{ .Number = 0x000000000008EC93, .String = "584851" },
    ChallengeID{ .Number = 0x0000000000092497, .String = "599191" },
    ChallengeID{ .Number = 0x000000000008F7D2, .String = "587730" },
    ChallengeID{ .Number = 0x000000000008EC5B, .String = "584795" },
    ChallengeID{ .Number = 0x000000000008F0FF, .String = "585983" },
    ChallengeID{ .Number = 0x0000000000091CAC, .String = "597164" },
    ChallengeID{ .Number = 0x000000000008DB71, .String = "580465" },
    ChallengeID{ .Number = 0x0000000000091C9A, .String = "597146" },
    ChallengeID{ .Number = 0x0000000000092675, .String = "599669" },
    ChallengeID{ .Number = 0x000000000008EF03, .String = "585475" },
    ChallengeID{ .Number = 0x0000000000092A4D, .String = "600653" },
    ChallengeID{ .Number = 0x000000000008F7CE, .String = "587726" },
    ChallengeID{ .Number = 0x0000000000091FD6, .String = "597974" },
    ChallengeID{ .Number = 0x0000000000091D06, .String = "597254" },
    ChallengeID{ .Number = 0x000000000008EEC7, .String = "585415" },
    ChallengeID{ .Number = 0x000000000008F836, .String = "587830" },
    ChallengeID{ .Number = 0x00000000000924A3, .String = "599203" },
    ChallengeID{ .Number = 0x0000000000092826, .String = "600102" },
    ChallengeID{ .Number = 0x000000000008F89B, .String = "587931" },
    ChallengeID{ .Number = 0x000000000009264E, .String = "599630" },
    ChallengeID{ .Number = 0x000000000009284A, .String = "600138" },
    ChallengeID{ .Number = 0x000000000008F7A4, .String = "587684" },
    ChallengeID{ .Number = 0x0000000000092895, .String = "600213" },
    ChallengeID{ .Number = 0x000000000008F5C1, .String = "587201" },
    ChallengeID{ .Number = 0x00000000000921E0, .String = "598496" },
    ChallengeID{ .Number = 0x000000000008F82E, .String = "587822" },
    ChallengeID{ .Number = 0x0000000000092928, .String = "600360" },
    ChallengeID{ .Number = 0x00000000000923FD, .String = "599037" },
    ChallengeID{ .Number = 0x000000000008F7C8, .String = "587720" },
    ChallengeID{ .Number = 0x0000000000091BED, .String = "596973" },
    ChallengeID{ .Number = 0x00000000000950B0, .String = "610480" },
    ChallengeID{ .Number = 0x000000000009252B, .String = "599339" },
    ChallengeID{ .Number = 0x000000000008F897, .String = "587927" },
    ChallengeID{ .Number = 0x0000000000091D9A, .String = "597402" },
    ChallengeID{ .Number = 0x000000000008F21A, .String = "586266" },
    ChallengeID{ .Number = 0x00000000000937CD, .String = "604109" },
    ChallengeID{ .Number = 0x000000000008E36C, .String = "582508" },
    ChallengeID{ .Number = 0x00000000000928E8, .String = "600296" },
    ChallengeID{ .Number = 0x000000000008EE99, .String = "585369" },
    ChallengeID{ .Number = 0x000000000008DCC7, .String = "580807" },
    ChallengeID{ .Number = 0x000000000008DCD0, .String = "580816" },
    ChallengeID{ .Number = 0x000000000008E3C1, .String = "582593" },
    ChallengeID{ .Number = 0x00000000000925AF, .String = "599471" },
    ChallengeID{ .Number = 0x000000000008F851, .String = "587857" },
    ChallengeID{ .Number = 0x000000000008EC34, .String = "584756" },
    ChallengeID{ .Number = 0x000000000008ED14, .String = "584980" },
    ChallengeID{ .Number = 0x000000000009253F, .String = "599359" },
    ChallengeID{ .Number = 0x0000000000093EE5, .String = "605925" },
    ChallengeID{ .Number = 0x00000000000929FD, .String = "600573" },
    ChallengeID{ .Number = 0x000000000008ECBE, .String = "584894" },
    ChallengeID{ .Number = 0x000000000008E3E7, .String = "582631" },
    ChallengeID{ .Number = 0x00000000000924C7, .String = "599239" },
    ChallengeID{ .Number = 0x000000000008DCB2, .String = "580786" },
    ChallengeID{ .Number = 0x0000000000092876, .String = "600182" },
    ChallengeID{ .Number = 0x0000000000092632, .String = "599602" },
    ChallengeID{ .Number = 0x00000000000957C7, .String = "612295" },
    ChallengeID{ .Number = 0x0000000000091D8F, .String = "597391" },
    ChallengeID{ .Number = 0x000000000008F1E4, .String = "586212" },
    ChallengeID{ .Number = 0x000000000008DCD5, .String = "580821" },
    ChallengeID{ .Number = 0x000000000008ECE9, .String = "584937" },
    ChallengeID{ .Number = 0x0000000000091BC4, .String = "596932" },
    ChallengeID{ .Number = 0x0000000000092A1F, .String = "600607" },
    ChallengeID{ .Number = 0x000000000008F0F4, .String = "585972" },
    ChallengeID{ .Number = 0x0000000000091DB3, .String = "597427" },
    ChallengeID{ .Number = 0x00000000000927C2, .String = "600002" },
    ChallengeID{ .Number = 0x000000000008F641, .String = "587329" },
    ChallengeID{ .Number = 0x0000000000093F09, .String = "605961" },
    ChallengeID{ .Number = 0x0000000000093F15, .String = "605973" },
    ChallengeID{ .Number = 0x0000000000091FE2, .String = "597986" },
    ChallengeID{ .Number = 0x000000000008F842, .String = "587842" },
    ChallengeID{ .Number = 0x0000000000092025, .String = "598053" },
    ChallengeID{ .Number = 0x000000000008ECD0, .String = "584912" },
    ChallengeID{ .Number = 0x000000000008F1D7, .String = "586199" },
    ChallengeID{ .Number = 0x000000000008F726, .String = "587558" },
    ChallengeID{ .Number = 0x000000000008F83E, .String = "587838" },
    ChallengeID{ .Number = 0x000000000008DCC1, .String = "580801" },
    ChallengeID{ .Number = 0x00000000000950A5, .String = "610469" },
    ChallengeID{ .Number = 0x00000000000936C2, .String = "603842" },
    ChallengeID{ .Number = 0x000000000008ECCA, .String = "584906" },
    ChallengeID{ .Number = 0x0000000000093F0D, .String = "605965" },
    ChallengeID{ .Number = 0x0000000000091C2C, .String = "597036" },
    ChallengeID{ .Number = 0x000000000008F4C0, .String = "586944" },
    ChallengeID{ .Number = 0x0000000000091DC7, .String = "597447" },
    ChallengeID{ .Number = 0x000000000008F0FB, .String = "585979" },
    ChallengeID{ .Number = 0x0000000000096C95, .String = "617621" },
    ChallengeID{ .Number = 0x00000000000928BF, .String = "600255" },
    ChallengeID{ .Number = 0x000000000008EC78, .String = "584824" },
    ChallengeID{ .Number = 0x000000000008ECB7, .String = "584887" },
    ChallengeID{ .Number = 0x000000000008F260, .String = "586336" },
    ChallengeID{ .Number = 0x0000000000091FEE, .String = "597998" },
    ChallengeID{ .Number = 0x000000000008E5F5, .String = "583157" },
    ChallengeID{ .Number = 0x000000000009254B, .String = "599371" },
    ChallengeID{ .Number = 0x0000000000092524, .String = "599332" },
    ChallengeID{ .Number = 0x000000000008E373, .String = "582515" },
    ChallengeID{ .Number = 0x0000000000091E1E, .String = "597534" },
    ChallengeID{ .Number = 0x000000000008EE9F, .String = "585375" },
    ChallengeID{ .Number = 0x00000000000929DA, .String = "600538" },
    ChallengeID{ .Number = 0x000000000008F0D7, .String = "585943" },
    ChallengeID{ .Number = 0x0000000000091FCA, .String = "597962" },
    ChallengeID{ .Number = 0x0000000000092530, .String = "599344" },
    ChallengeID{ .Number = 0x000000000008ECFF, .String = "584959" },
    ChallengeID{ .Number = 0x000000000008EEC0, .String = "585408" },
    ChallengeID{ .Number = 0x0000000000091E17, .String = "597527" },
    ChallengeID{ .Number = 0x000000000008F201, .String = "586241" },
    ChallengeID{ .Number = 0x000000000008EE5E, .String = "585310" },
    ChallengeID{ .Number = 0x000000000008ECB0, .String = "584880" },
    ChallengeID{ .Number = 0x000000000008F0E2, .String = "585954" },
    ChallengeID{ .Number = 0x00000000000925AA, .String = "599466" },
    ChallengeID{ .Number = 0x000000000009260C, .String = "599564" },
    ChallengeID{ .Number = 0x000000000008EE00, .String = "585216" },
    ChallengeID{ .Number = 0x00000000000925FE, .String = "599550" },
    ChallengeID{ .Number = 0x0000000000092A50, .String = "600656" },
    ChallengeID{ .Number = 0x000000000008F0CE, .String = "585934" },
    ChallengeID{ .Number = 0x000000000008EC9E, .String = "584862" },
    ChallengeID{ .Number = 0x0000000000092882, .String = "600194" },
    ChallengeID{ .Number = 0x0000000000091D44, .String = "597316" },
    ChallengeID{ .Number = 0x00000000000924E1, .String = "599265" },
    ChallengeID{ .Number = 0x00000000000926A0, .String = "599712" },
    ChallengeID{ .Number = 0x000000000008DB74, .String = "580468" },
    ChallengeID{ .Number = 0x000000000009260F, .String = "599567" },
    ChallengeID{ .Number = 0x00000000000923EB, .String = "599019" },
    ChallengeID{ .Number = 0x000000000008E8C7, .String = "583879" },
    ChallengeID{ .Number = 0x00000000000932BB, .String = "602811" },
    ChallengeID{ .Number = 0x000000000008F20C, .String = "586252" },
    ChallengeID{ .Number = 0x00000000000926A8, .String = "599720" },
    ChallengeID{ .Number = 0x000000000009285C, .String = "600156" },
    ChallengeID{ .Number = 0x000000000008F86E, .String = "587886" },
    ChallengeID{ .Number = 0x0000000000091D14, .String = "597268" },
    ChallengeID{ .Number = 0x000000000008E5D7, .String = "583127" },
    ChallengeID{ .Number = 0x000000000009286A, .String = "600170" },
    ChallengeID{ .Number = 0x000000000009289C, .String = "600220" },
    ChallengeID{ .Number = 0x000000000008E3E2, .String = "582626" },
    ChallengeID{ .Number = 0x0000000000092002, .String = "598018" },
    ChallengeID{ .Number = 0x000000000008EC21, .String = "584737" },
    ChallengeID{ .Number = 0x0000000000092516, .String = "599318" },
    ChallengeID{ .Number = 0x000000000008DAFB, .String = "580347" },
    ChallengeID{ .Number = 0x00000000000927F7, .String = "600055" },
    ChallengeID{ .Number = 0x00000000000950AB, .String = "610475" },
    ChallengeID{ .Number = 0x000000000008EB91, .String = "584593" },
    ChallengeID{ .Number = 0x0000000000092041, .String = "598081" },
    ChallengeID{ .Number = 0x00000000000928C3, .String = "600259" },
    ChallengeID{ .Number = 0x0000000000092656, .String = "599638" },
    ChallengeID{ .Number = 0x000000000009210A, .String = "598282" },
    ChallengeID{ .Number = 0x00000000000927B4, .String = "599988" },
    ChallengeID{ .Number = 0x000000000008F85B, .String = "587867" },
    ChallengeID{ .Number = 0x000000000008E3C8, .String = "582600" },
    ChallengeID{ .Number = 0x000000000008F01B, .String = "585755" },
    ChallengeID{ .Number = 0x000000000008EEFC, .String = "585468" },
    ChallengeID{ .Number = 0x000000000008EBF2, .String = "584690" },
    ChallengeID{ .Number = 0x00000000000947B8, .String = "608184" },
    ChallengeID{ .Number = 0x00000000000920A7, .String = "598183" },
    ChallengeID{ .Number = 0x0000000000093EED, .String = "605933" },
    ChallengeID{ .Number = 0x000000000008F8F6, .String = "588022" },
    ChallengeID{ .Number = 0x000000000008F524, .String = "587044" },
    ChallengeID{ .Number = 0x000000000008EE7E, .String = "585342" },
    ChallengeID{ .Number = 0x0000000000092955, .String = "600405" },
    ChallengeID{ .Number = 0x0000000000092792, .String = "599954" },
    ChallengeID{ .Number = 0x00000000000929D7, .String = "600535" },
    ChallengeID{ .Number = 0x0000000000091DBA, .String = "597434" },
    ChallengeID{ .Number = 0x0000000000092554, .String = "599380" },
    ChallengeID{ .Number = 0x00000000000924A8, .String = "599208" },
    ChallengeID{ .Number = 0x0000000000091D18, .String = "597272" },
    ChallengeID{ .Number = 0x00000000000920F2, .String = "598258" },
    ChallengeID{ .Number = 0x00000000000965FF, .String = "615935" },
    ChallengeID{ .Number = 0x000000000008DCEC, .String = "580844" },
    ChallengeID{ .Number = 0x00000000000947C6, .String = "608198" },
    ChallengeID{ .Number = 0x000000000008F0F0, .String = "585968" },
    ChallengeID{ .Number = 0x0000000000092940, .String = "600384" },
    ChallengeID{ .Number = 0x0000000000096CA3, .String = "617635" },
    ChallengeID{ .Number = 0x000000000008F542, .String = "587074" },
    ChallengeID{ .Number = 0x000000000009209F, .String = "598175" },
    ChallengeID{ .Number = 0x00000000000920FD, .String = "598269" },
    ChallengeID{ .Number = 0x0000000000091C26, .String = "597030" },
    ChallengeID{ .Number = 0x000000000008F1CE, .String = "586190" },
    ChallengeID{ .Number = 0x0000000000091CEE, .String = "597230" },
    ChallengeID{ .Number = 0x0000000000091DD8, .String = "597464" },
    ChallengeID{ .Number = 0x000000000008EE58, .String = "585304" },
    ChallengeID{ .Number = 0x0000000000091FE6, .String = "597990" },
    ChallengeID{ .Number = 0x0000000000091BFB, .String = "596987" },
    ChallengeID{ .Number = 0x000000000008F7A0, .String = "587680" },
    ChallengeID{ .Number = 0x000000000009245B, .String = "599131" },
    ChallengeID{ .Number = 0x0000000000092A53, .String = "600659" },
    ChallengeID{ .Number = 0x00000000000932AD, .String = "602797" },
    ChallengeID{ .Number = 0x000000000008EE1C, .String = "585244" },
    ChallengeID{ .Number = 0x00000000000927A8, .String = "599976" },
    ChallengeID{ .Number = 0x0000000000092462, .String = "599138" },
    ChallengeID{ .Number = 0x000000000008F826, .String = "587814" },
    ChallengeID{ .Number = 0x0000000000091FDA, .String = "597978" },
    ChallengeID{ .Number = 0x0000000000093206, .String = "602630" },
    ChallengeID{ .Number = 0x000000000008F848, .String = "587848" },
    ChallengeID{ .Number = 0x000000000008EEAE, .String = "585390" },
    ChallengeID{ .Number = 0x000000000008F1DB, .String = "586203" },
    ChallengeID{ .Number = 0x00000000000920CA, .String = "598218" },
    ChallengeID{ .Number = 0x000000000008E3DA, .String = "582618" },
    ChallengeID{ .Number = 0x000000000008F866, .String = "587878" },
    ChallengeID{ .Number = 0x000000000008EE82, .String = "585346" },
    ChallengeID{ .Number = 0x000000000008F212, .String = "586258" },
    ChallengeID{ .Number = 0x000000000009294E, .String = "600398" },
    ChallengeID{ .Number = 0x00000000000928B3, .String = "600243" },
    ChallengeID{ .Number = 0x000000000008F0D2, .String = "585938" },
    ChallengeID{ .Number = 0x000000000008F88F, .String = "587919" },
    ChallengeID{ .Number = 0x0000000000092684, .String = "599684" },
    ChallengeID{ .Number = 0x000000000008DB8C, .String = "580492" },
    ChallengeID{ .Number = 0x0000000000092A19, .String = "600601" },
    ChallengeID{ .Number = 0x000000000008F0DD, .String = "585949" },
    ChallengeID{ .Number = 0x000000000008EF57, .String = "585559" },
    ChallengeID{ .Number = 0x000000000008F513, .String = "587027" },
    ChallengeID{ .Number = 0x000000000008EEB5, .String = "585397" },
    ChallengeID{ .Number = 0x00000000000970F8, .String = "618744" },
    ChallengeID{ .Number = 0x00000000000927C6, .String = "600006" },
    ChallengeID{ .Number = 0x000000000008ECC6, .String = "584902" },
    ChallengeID{ .Number = 0x000000000008E377, .String = "582519" },
    ChallengeID{ .Number = 0x00000000000924B9, .String = "599225" },
    ChallengeID{ .Number = 0x0000000000092404, .String = "599044" },
    ChallengeID{ .Number = 0x000000000008E3D6, .String = "582614" },
    ChallengeID{ .Number = 0x0000000000092846, .String = "600134" },
    ChallengeID{ .Number = 0x0000000000091D4B, .String = "597323" },
    ChallengeID{ .Number = 0x00000000000932CE, .String = "602830" },
    ChallengeID{ .Number = 0x000000000008F0CA, .String = "585930" },
    ChallengeID{ .Number = 0x00000000000929D0, .String = "600528" },
    ChallengeID{ .Number = 0x0000000000091DC2, .String = "597442" },
    ChallengeID{ .Number = 0x000000000009195F, .String = "596319" },
    ChallengeID{ .Number = 0x0000000000092534, .String = "599348" },
    ChallengeID{ .Number = 0x00000000000923E7, .String = "599015" },
    ChallengeID{ .Number = 0x0000000000094789, .String = "608137" },
    ChallengeID{ .Number = 0x00000000000947D4, .String = "608212" },
    ChallengeID{ .Number = 0x00000000000946DF, .String = "607967" },
    ChallengeID{ .Number = 0x00000000000932BF, .String = "602815" },
    ChallengeID{ .Number = 0x0000000000093F25, .String = "605989" },
    ChallengeID{ .Number = 0x00000000000923D4, .String = "598996" },
    ChallengeID{ .Number = 0x0000000000091B5B, .String = "596827" },
    ChallengeID{ .Number = 0x00000000000929C6, .String = "600518" },
    ChallengeID{ .Number = 0x0000000000092157, .String = "598359" },
    ChallengeID{ .Number = 0x000000000009241E, .String = "599070" },
    ChallengeID{ .Number = 0x000000000008EBED, .String = "584685" },
    ChallengeID{ .Number = 0x000000000008EBF8, .String = "584696" },
    ChallengeID{ .Number = 0x000000000009261F, .String = "599583" },
    ChallengeID{ .Number = 0x000000000008E5E6, .String = "583142" },
    ChallengeID{ .Number = 0x00000000000929A7, .String = "600487" },
    ChallengeID{ .Number = 0x000000000008ECAA, .String = "584874" },
    ChallengeID{ .Number = 0x00000000000932C5, .String = "602821" },
    ChallengeID{ .Number = 0x000000000008E3EE, .String = "582638" },
    ChallengeID{ .Number = 0x0000000000092911, .String = "600337" },
    ChallengeID{ .Number = 0x0000000000094F7D, .String = "610173" },
    ChallengeID{ .Number = 0x00000000000929A0, .String = "600480" },
    ChallengeID{ .Number = 0x0000000000093F11, .String = "605969" },
    ChallengeID{ .Number = 0x000000000008EC2C, .String = "584748" },
    ChallengeID{ .Number = 0x0000000000093F21, .String = "605985" },
    ChallengeID{ .Number = 0x00000000000920BB, .String = "598203" },
    ChallengeID{ .Number = 0x00000000000932B6, .String = "602806" },
    ChallengeID{ .Number = 0x000000000008ECC2, .String = "584898" },
    ChallengeID{ .Number = 0x00000000000C695C, .String = "813404" },
    ChallengeID{ .Number = 0x00000000000C6A49, .String = "813641" },
    ChallengeID{ .Number = 0x00000000000C708D, .String = "815245" },
    ChallengeID{ .Number = 0x00000000000C6988, .String = "813448" },
    ChallengeID{ .Number = 0x00000000000C6A06, .String = "813574" },
    ChallengeID{ .Number = 0x00000000000C6974, .String = "813428" },
    ChallengeID{ .Number = 0x00000000000C69EF, .String = "813551" },
    ChallengeID{ .Number = 0x00000000000C6A2E, .String = "813614" },
    ChallengeID{ .Number = 0x00000000000C6A35, .String = "813621" },
    ChallengeID{ .Number = 0x00000000000C69B2, .String = "813490" },
    ChallengeID{ .Number = 0x00000000000BA10E, .String = "762126" },
    ChallengeID{ .Number = 0x00000000000BA116, .String = "762134" },
    ChallengeID{ .Number = 0x00000000000BA106, .String = "762118" },
    ChallengeID{ .Number = 0x00000000000BA0F9, .String = "762105" },
    ChallengeID{ .Number = 0x00000000000BA131, .String = "762161" },
    ChallengeID{ .Number = 0x00000000000BA12A, .String = "762154" },
    ChallengeID{ .Number = 0x00000000000BA11E, .String = "762142" },
    ChallengeID{ .Number = 0x00000000000BA126, .String = "762150" },
    ChallengeID{ .Number = 0x00000000000BA0C7, .String = "762055" },
    ChallengeID{ .Number = 0x00000000000BA0CB, .String = "762059" },
    ChallengeID{ .Number = 0x00000000000BA0BB, .String = "762043" },
    ChallengeID{ .Number = 0x00000000000BA0C2, .String = "762050" },
    ChallengeID{ .Number = 0x00000000000BA0E4, .String = "762084" },
    ChallengeID{ .Number = 0x00000000000BA0EF, .String = "762095" },
    ChallengeID{ .Number = 0x00000000000BA0D2, .String = "762066" },
    ChallengeID{ .Number = 0x00000000000BA0DD, .String = "762077" },
    ChallengeID{ .Number = 0x00000000000BA138, .String = "762168" },
    ChallengeID{ .Number = 0x00000000000BA170, .String = "762224" },
    ChallengeID{ .Number = 0x00000000000BA178, .String = "762232" },
    ChallengeID{ .Number = 0x00000000000BA168, .String = "762216" },
    ChallengeID{ .Number = 0x00000000000BA16C, .String = "762220" },
    ChallengeID{ .Number = 0x0000000000093F19, .String = "605977" },
    ChallengeID{ .Number = 0x00000000000BA07E, .String = "761982" },
    ChallengeID{ .Number = 0x00000000000BA183, .String = "762243" },
    ChallengeID{ .Number = 0x0000000000093F1D, .String = "605981" },
    ChallengeID{ .Number = 0x00000000000BA148, .String = "762184" },
    ChallengeID{ .Number = 0x00000000000BA14D, .String = "762189" },
    ChallengeID{ .Number = 0x00000000000BA13F, .String = "762175" },
    ChallengeID{ .Number = 0x00000000000BA143, .String = "762179" },
    ChallengeID{ .Number = 0x00000000000BA15D, .String = "762205" },
    ChallengeID{ .Number = 0x00000000000BA164, .String = "762212" },
    ChallengeID{ .Number = 0x00000000000BA151, .String = "762193" },
    ChallengeID{ .Number = 0x00000000000BA155, .String = "762197" },
    ChallengeID{ .Number = 0x00000000000BA0B1, .String = "762033" },
    ChallengeID{ .Number = 0x00000000000B9FF7, .String = "761847" },
    ChallengeID{ .Number = 0x00000000000B9FFE, .String = "761854" },
    ChallengeID{ .Number = 0x00000000000B9FF0, .String = "761840" },
    ChallengeID{ .Number = 0x00000000000B9FE4, .String = "761828" },
    ChallengeID{ .Number = 0x00000000000B9FEB, .String = "761835" },
    ChallengeID{ .Number = 0x00000000000BA021, .String = "761889" },
    ChallengeID{ .Number = 0x00000000000BA029, .String = "761897" },
    ChallengeID{ .Number = 0x00000000000BA013, .String = "761875" },
    ChallengeID{ .Number = 0x00000000000BA005, .String = "761861" },
    ChallengeID{ .Number = 0x00000000000BA009, .String = "761865" },
    ChallengeID{ .Number = 0x00000000000B9F70, .String = "761712" },
    ChallengeID{ .Number = 0x00000000000B9F76, .String = "761718" },
    ChallengeID{ .Number = 0x00000000000B9F66, .String = "761702" },
    ChallengeID{ .Number = 0x00000000000947D9, .String = "608217" },
    ChallengeID{ .Number = 0x00000000000B9E01, .String = "761345" },
    ChallengeID{ .Number = 0x00000000000B9FD4, .String = "761812" },
    ChallengeID{ .Number = 0x00000000000B9FDD, .String = "761821" },
    ChallengeID{ .Number = 0x00000000000B9FCD, .String = "761805" },
    ChallengeID{ .Number = 0x00000000000B9FAB, .String = "761771" },
    ChallengeID{ .Number = 0x00000000000B9FBD, .String = "761789" },
    ChallengeID{ .Number = 0x00000000000947BD, .String = "608189" },
    ChallengeID{ .Number = 0x00000000000BA085, .String = "761989" },
    ChallengeID{ .Number = 0x00000000000BA07A, .String = "761978" },
    ChallengeID{ .Number = 0x00000000000BA069, .String = "761961" },
    ChallengeID{ .Number = 0x00000000000BA073, .String = "761971" },
    ChallengeID{ .Number = 0x00000000000BA0A6, .String = "762022" },
    ChallengeID{ .Number = 0x00000000000BA0AA, .String = "762026" },
    ChallengeID{ .Number = 0x00000000000BA0A2, .String = "762018" },
    ChallengeID{ .Number = 0x00000000000BA08C, .String = "761996" },
    ChallengeID{ .Number = 0x00000000000BA093, .String = "762003" },
    ChallengeID{ .Number = 0x00000000000BA044, .String = "761924" },
    ChallengeID{ .Number = 0x00000000000BA04B, .String = "761931" },
    ChallengeID{ .Number = 0x00000000000BA03E, .String = "761918" },
    ChallengeID{ .Number = 0x00000000000BA035, .String = "761909" },
    ChallengeID{ .Number = 0x00000000000BA039, .String = "761913" },
    ChallengeID{ .Number = 0x00000000000BA05E, .String = "761950" },
    ChallengeID{ .Number = 0x00000000000BA062, .String = "761954" },
    ChallengeID{ .Number = 0x00000000000BA05A, .String = "761946" },
    ChallengeID{ .Number = 0x00000000000BA04F, .String = "761935" },
    ChallengeID{ .Number = 0x00000000000BA056, .String = "761942" },
};


inline const ChallengeID* GetVanillaChallengeID(uint64_t challengeID)
{
    for (const ChallengeID& vanillaChallengeID : k_VanillaChallengeIDs)
    {
        if (vanillaChallengeID.Number == challengeID)
        {
            return &vanillaChallengeID;
        }
    }

    return nullptr;
}