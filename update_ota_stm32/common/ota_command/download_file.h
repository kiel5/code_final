#ifndef DOWNLOAD_FILE_H
#define DOWNLOAD_FILE_H
#include "stdint.h"
#include "string.h"
#include "stdlib.h"

enum{
    status_ok,
    status_error
};

char file_hex[1024*50] = 
R"(
:020000040800F2
:105000000050002061530008055300080B530008AE
:1050100011530008175300081D530008000000003A
:105020000000000000000000000000002353000802
:105030002F530008000000003B53000847530008AE
:10504000A9530008A9530008A9530008A953000850
:10505000A9530008A9530008A9530008A953000840
:10506000A9530008A9530008A9530008A953000830
:10507000A9530008A9530008A9530008A953000820
:10508000A9530008A9530008A9530008A953000810
:10509000A9530008A9530008A9530008A953000800
:1050A000A9530008A9530008A9530008A9530008F0
:1050B000A9530008A9530008A9530008A9530008E0
:1050C000A9530008A9530008A9530008A9530008D0
:1050D000A9530008A9530008A9530008A9530008C0
:1050E000A9530008A9530008A953000800000000B4
:1050F00000000000000000000000000000000000B0
:0C51000000000000000000005FF808F153
:10510C0010B5054C237833B9044B13B10448AFF3F5
:10511C0000800123237010BD0C0000200000000053
:10512C000062000808B5034B1BB103490348AFF3F9
:10513C00008008BD00000000100000200062000884
:10514C0080B500AF094B0A4A9A6000F029F900F0CB
:10515C0013F800F04DF84FF40051064800F026FC0F
:10516C004FF47A7000F07EF9F5E700BF00ED00E037
:10517C00005000080010014080B590B000AF07F15E
:10518C00180328220021184601F02CF83B1D0022A0
:10519C001A605A609A60DA601A610223BB610123BB
:1051AC00BB621023FB6200237B6307F118031846D4
:1051BC0000F016FC0346002B01D000F065F80F231D
:1051CC007B600023BB600023FB6000233B6100235A
:1051DC007B613B1D0021184600F084FE0346002B2A
:1051EC0001D000F051F800BF4037BD4680BD000033
:1051FC0080B588B000AF07F1100300221A605A6026
:10520C009A60DA601E4B9B691D4A43F01003936150
:10521C001B4B9B6903F01003FB60FB68184B9B69ED
:10522C00174A43F020039361154B9B6903F020034D
:10523C00BB60BB68124B9B69114A43F0040393613A
:10524C000F4B9B6903F004037B607B6800224FF4D7
:10525C0000510C4800F092FB4FF400533B610123CA
:10526C007B610023BB610223FB6107F1100319462C
:10527C00044800F0FFF900BF2037BD4680BD00BFD9
:10528C00001002400010014080B400AF72B600BFA5
:10529C00FEE7000080B485B000AF154B9B69144A43
:1052AC0043F001039361124B9B6903F00103BB6054
:1052BC00BB680F4BDB690E4A43F08053D3610C4B38
:1052CC00DB6903F080537B607B680A4B5B68FB6097
:1052DC00FB6823F0E063FB60FB6843F00073FB604A
:1052EC00044AFB68536000BF1437BD4680BC70474E
:1052FC00001002400000014080B400AFFEE780B413
:10530C0000AFFEE780B400AFFEE780B400AFFEE76D
:10531C0080B400AFFEE780B400AF00BFBD4680BCD8
:10532C00704780B400AF00BFBD4680BC704780B4EE
:10533C0000AF00BFBD4680BC704780B500AF00F029
:10534C0075F800BF80BD80B400AF00BFBD4680BC07
:10535C0070470000FFF7F7FF0B480C490C4A00237D
:10536C0002E0D458C4500433C4188C42F9D3094A0F
:10537C00094C002301E013600432A242FBD300F07D
:10538C000DFFFFF7DDFE7047000000200C00002031
:10539C00446200080C0000202C000020FEE70000F6
:1053AC0080B500AF084B1B68074A43F0100313602D
:1053BC00032000F02BF90F2000F008F8FFF76AFF2C
:1053CC000023184680BD00BF0020024080B582B08B
:1053DC0000AF7860124B1A68124B1B7819464FF4C9
:1053EC007A73B3FBF1F3B2FBF3F3184600F035F923
:1053FC000346002B01D001230EE07B680F2B0AD84B
:10540C00002279684FF0FF3000F00BF9064A7B68F8
:10541C001360002300E0012318460837BD4680BD09
:10542C0000000020080000200400002080B400AF21
:10543C00054B1B781A46054B1B681344034A136033
:10544C0000BFBD4680BC704708000020280000202B
:10545C0080B400AF024B1B681846BD4680BC704739
:10546C002800002080B584B000AF7860FFF7F0FF13
:10547C00B8607B68FB60FB68B3F1FF3F05D00A4B5B
:10548C001B781A46FB681344FB6000BFFFF7E0FF74
:10549C000246BB68D31AFA689A42F7D800BF00BF1D
:1054AC001037BD4680BD00BF0800002080B485B019
:1054BC0000AF78607B6803F00703FB600C4BDB6884
:1054CC00BB60BA684FF6FF031340BB60FB681A025F
:1054DC00BB68134343F0BF6343F40033BB60044A1F
:1054EC00BB68D36000BF1437BD4680BC704700BF9B
:1054FC0000ED00E080B400AF044BDB681B0A03F046
:10550C0007031846BD4680BC704700BF00ED00E0A5
:10551C0080B483B000AF03463960FB7197F9073054
:10552C00002B0ADB3B68DAB20C4997F90730120101
:10553C00D2B20B4483F800230AE03B68DAB2084984
:10554C00FB7903F00F03043B1201D2B20B441A7621
:10555C0000BF0C37BD4680BC704700BF00E100E0C7
:10556C0000ED00E080B489B000AFF860B9607A60FB
:10557C00FB6803F00703FB61FB69C3F10703042B12
:10558C0028BF0423BB61FB690433062B02D9FB69DA
:10559C00033B00E000237B614FF0FF32BB6902FA52
:1055AC0003F3DA43BB681A407B699A404FF0FF3132
:1055BC007B6901FA03F3D9437B680B40134318460C
:1055CC002437BD4680BC704780B582B000AF786090
:1055DC007B68013BB3F1807F01D301230FE00A4AC2
:1055EC007B68013B53600F214FF0FF30FFF790FFBA
:1055FC00054B00229A60044B07221A6000231846C0
:10560C000837BD4680BD00BF10E000E080B582B019
:10561C0000AF78607868FFF749FF00BF0837BD46D8
:10562C0080BD80B586B000AF0346B9607A60FB736D
:10563C0000237B61FFF75EFF78617A68B96878694F
:10564C00FFF790FF024697F90F3011461846FFF707
:10565C005FFF00BF1837BD4680BD80B582B000AF7C
:10566C0078607868FFF7B0FF034618460837BD46E8
:10567C0080BD000080B48BB000AF7860396000232F
:10568C007B6200233B6269E101227B6A02FA03F32D
:10569C00FB613B681B68FA691340BB61BA69FB6923
:1056AC009A4240F058813B685B689A4A93425ED0BC
:1056BC00984A934275D8984A934258D0964A934246
:1056CC006FD8964A934252D0944A934269D8944ADE
:1056DC0093424CD0924A934263D8924A934246D01A
:1056EC00904A93425DD8122B2AD8122B59D801A27A
:1056FC0052F823F07B5700085557000867570008ED
:10570C00A9570008AF570008AF570008AF5700085B
:10571C00AF570008AF570008AF570008AF57000845
:10572C00AF570008AF570008AF570008AF57000835
:10573C00AF570008AF5700085D57000871570008B5
:10574C00794A934213D02CE03B68DB683B6229E03A
:10575C003B68DB6804333B6224E03B68DB6808335E
:10576C003B621FE03B68DB680C333B621AE03B6832
:10577C009B68002B02D104233B6213E03B689B68BF
:10578C00012B05D108233B627B68FA691A6109E099
:10579C0008233B627B68FA695A6103E000233B6291
:1057AC0000E000BFBB69FF2B01D87B6801E07B6880
:1057BC0004337B61BB69FF2B02D87B6A9B0002E040
:1057CC007B6A083B9B003B617B691A680F213B6934
:1057DC0001FA03F3DB431A40396A3B6901FA03F31C
:1057EC001A437B691A603B685B6803F08053002B9B
:1057FC0000F0B1804D4B9B694C4A43F0010393611F
:10580C004A4B9B6903F00103BB60BB68484A7B6A47
:10581C009B08023352F82330FB607B6A03F00303CE
:10582C009B000F2202FA03F3DB43FA681340FB6080
:10583C007B68404A934213D07B683F4A93420DD019
:10584C007B683E4A934207D07B683D4A934201D124
:10585C00032306E0042304E0022302E0012300E01A
:10586C0000237A6A02F0030292009340FA68134311
:10587C00FB602F497B6A9B080233FA6841F82320AE
:10588C003B685B6803F48013002B06D02D4B9A68A1
:10589C002C49BB6913438B6006E02A4B9A68BB69A1
:1058AC00DB43284913408B603B685B6803F40013AF
:1058BC00002B06D0234BDA682249BB691343CB601B
:1058CC0006E0204BDA68BB69DB431E491340CB6012
:1058DC003B685B6803F40033002B06D0194B5A6805
:1058EC001849BB6913434B6006E0164B5A68BB69F9
:1058FC00DB43144913404B603B685B6803F4803313
:10590C00002B21D00F4B1A680E49BB6913430B6057
:10591C0021E000BF00003210000031100000221006
:10592C0000002110000012100000111000100240A5
:10593C000000014000080140000C01400010014033
:10594C0000140140000401400B4B1A68BB69DB4397
:10595C00094913400B607B6A01337B623B681A6810
:10596C007B6A22FA03F3002B7FF48EAE00BF00BFDC
:10597C002C37BD4680BC70470004014080B483B016
:10598C0000AF78600B467B8013467B707B78002BD6
:10599C0003D07A887B681A6103E07B881A047B68E1
:1059AC001A6100BF0C37BD4680BC704780B485B00F
:1059BC0000AF78600B467B807B68DB68FB607A8885
:1059CC00FB6813401A04FB68D9437B880B401A43CD
:1059DC007B681A6100BF1437BD4680BC704700005D
:1059EC0080B586B000AF78607B68002B01D10123B5
:1059FC0072E27B681B6803F00103002B00F08780C8
:105A0C00924B5B6803F00C03042B0CD08F4B5B6840
:105A1C0003F00C03082B12D18C4B5B6803F480331E
:105A2C00B3F5803F0BD1894B1B6803F40033002B7B
:105A3C006CD07B685B68002B68D101234CE27B68DF
:105A4C005B68B3F5803F06D1804B1B687F4A43F4FB
:105A5C00803313602EE07B685B68002B0CD17B4B92
:105A6C001B687A4A23F480331360784B1B68774A9F
:105A7C0023F4802313601DE07B685B68B3F5A02FD3
:105A8C000CD1724B1B68714A43F4802313606F4B2B
:105A9C001B686E4A43F4803313600BE06B4B1B683E
:105AAC006A4A23F480331360684B1B68674A23F4FB
:105ABC00802313607B685B68002B13D0FFF7C8FC56
:105ACC00386108E0FFF7C4FC02463B69D31A642B2B
:105ADC0001D9032300E25D4B1B6803F40033002B58
:105AEC00F0D014E0FFF7B4FC386108E0FFF7B0FC2D
:105AFC0002463B69D31A642B01D90323ECE1534BC7
:105B0C001B6803F40033002BF0D100E000BF7B686E
:105B1C001B6803F00203002B63D04C4B5B6803F053
:105B2C000C03002B0BD0494B5B6803F00C03082BC8
:105B3C001CD1464B5B6803F48033002B16D1434BCE
:105B4C001B6803F00203002B05D07B681B69012B3B
:105B5C0001D00123C0E13D4B1B6823F0F8027B68A8
:105B6C005B69DB00394913430B603AE07B681B69C6
:105B7C00002B20D0364B01221A60FFF769FC3861EC
:105B8C0008E0FFF765FC02463B69D31A022B01D9EA
:105B9C000323A1E12D4B1B6803F00203002BF0D073
:105BAC002A4B1B6823F0F8027B685B69DB002749F2
:105BBC0013430B6015E0264B00221A60FFF748FCDC
:105BCC00386108E0FFF744FC02463B69D31A022B0C
:105BDC0001D9032380E11D4B1B6803F00203002B4A
:105BEC00F0D17B681B6803F00803002B3AD07B686C
:105BFC009B69002B19D0174B01221A60FFF728FC68
:105C0C00386108E0FFF724FC02463B69D31A022BEB
:105C1C0001D9032360E10D4B5B6A03F00203002BF7
:105C2C00F0D0012000F09CFA1CE00A4B00221A6014
:105C3C00FFF70EFC38610FE0FFF70AFC02463B69E8
:105C4C00D31A022B08D9032346E100BF00100240EF
:105C5C000000424280044242924B5B6A03F0020312
:105C6C00002BE9D17B681B6803F00403002B00F0C8
:105C7C00A6800023FB758B4BDB6903F08053002B54
:105C8C000DD1884BDB69874A43F08053D361854B38
:105C9C00DB6903F08053BB60BB680123FB75824B4F
:105CAC001B6803F48073002B18D17F4B1B687E4A52
:105CBC0043F480731360FFF7CBFB386108E0FFF708
:105CCC00C7FB02463B69D31A642B01D9032303E1BA
:105CDC00754B1B6803F48073002BF0D07B68DB687A
:105CEC00012B06D16F4B1B6A6E4A43F00103136202
:105CFC002DE07B68DB68002B0CD16A4B1B6A694A70
:105D0C0023F001031362674B1B6A664A23F00403FA
:105D1C0013621CE07B68DB68052B0CD1614B1B6AA2
:105D2C00604A43F0040313625E4B1B6A5D4A43F006
:105D3C00010313620BE05B4B1B6A5A4A23F001030D
:105D4C001362584B1B6A574A23F0040313627B6897
:105D5C00DB68002B15D0FFF77BFB38610AE0FFF7FF
:105D6C0077FB02463B69D31A41F28832934201D940
:105D7C000323B1E04B4B1B6A03F00203002BEED064
:105D8C0014E0FFF765FB38610AE0FFF761FB0246A0
:105D9C003B69D31A41F28832934201D903239BE029
:105DAC00404B1B6A03F00203002BEED1FB7D012B51
:105DBC0005D13C4BDB693B4A23F08053D3617B68B4
:105DCC00DB69002B00F08780364B5B6803F00C031B
:105DDC00082B61D07B68DB69022B46D1334B002248
:105DEC001A60FFF735FB386108E0FFF731FB02461C
:105DFC003B69D31A022B01D903236DE0294B1B6895
:105E0C0003F00073002BF0D17B681B6AB3F5803F65
:105E1C0008D1244B5B6823F400327B689B682149D2
:105E2C0013434B601F4B5B6823F474127B68196A35
:105E3C007B685B6A0B431B4913434B601B4B012272
:105E4C001A60FFF705FB386108E0FFF701FB02461B
:105E5C003B69D31A022B01D903233DE0114B1B687C
:105E6C0003F00073002BF0D035E0104B00221A60C9
:105E7C00FFF7EEFA386108E0FFF7EAFA02463B69F1
:105E8C00D31A022B01D9032326E0064B1B6803F01F
:105E9C000073002BF0D11EE07B68DB69012B07D16E
:105EAC00012319E0001002400070004060004242E3
:105EBC000B4B5B68FB60FB6803F480327B681B6AEE
:105ECC009A4206D1FB6803F470127B685B6A9A42B3
:105EDC0001D0012300E0002318461837BD4680BDD1
:105EEC000010024080B584B000AF786039607B68E8
:105EFC00002B01D10123D0E06A4B1B6803F0070390
:105F0C003A689A4210D9674B1B6823F0070265491F
:105F1C003B6813430B60634B1B6803F007033A6841
:105F2C009A4201D00123B8E07B681B6803F002039E
:105F3C00002B20D07B681B6803F00403002B05D0DA
:105F4C00594B5B68584A43F4E06353607B681B68A9
:105F5C0003F00803002B05D0534B5B68524A43F403
:105F6C0060535360504B5B6823F0F0027B689B6876
:105F7C004D4913434B607B681B6803F00103002BF6
:105F8C0040D07B685B68012B07D1474B1B6803F43F
:105F9C000033002B15D101237FE07B685B68022B5B
:105FAC0007D1414B1B6803F00073002B09D101236F
:105FBC0073E03D4B1B6803F00203002B01D101235E
:105FCC006BE0394B5B6823F003027B685B683649F6
:105FDC0013434B60FFF73CFAF8600AE0FFF738FA1E
:105FEC000246FB68D31A41F28832934201D903234B
:105FFC0053E02D4B5B6803F00C027B685B689B00E5
:10600C009A42EBD1274B1B6803F007033A689A427C
:10601C0010D2244B1B6823F0070222493B68134320
:10602C000B60204B1B6803F007033A689A4201D0BF
:10603C00012332E07B681B6803F00403002B08D0BB
:10604C00194B5B6823F4E0627B68DB6816491343E9
:10605C004B607B681B6803F00803002B09D0124BC4
:10606C005B6823F460527B681B69DB000E491343A9
:10607C004B6000F021F802460B4B5B681B0903F0E8
:10608C000F030A49CB5C22FA03F3094A1360094B4C
:10609C001B681846FFF79AF9002318461037BD46BF
:1060AC0080BD00BF002002400010024018620008B2
:1060BC00000000200400002080B487B000AF002353
:1060CC00FB600023BB6000237B6100237B6000230B
:1060DC003B611E4B5B68FB60FB6803F00C03042BFD
:1060EC0002D0082B03D027E0194B3B6127E0FB685B
:1060FC009B0C03F00F03174AD35C7B60FB6803F423
:10610C008033002B10D0114B5B685B0C03F0010348
:10611C00114AD35CBB607B680D4A03FB02F2BB687F
:10612C00B2FBF3F37B6104E07B680C4A02FB03F3E4
:10613C007B617B693B6102E0054B3B6100BF3B69C6
:10614C0018461C37BD4680BC704700BF001002408B
:10615C0000127A00286200083862000800093D002D
:10616C0080B485B000AF78600A4B1B680A4AA2FB6A
:10617C0003235B0A7A6802FB03F3FB6000BFFB6836
:10618C005A1EFA60002BF9D100BF00BF1437BD4670
:10619C0080BC704700000020D34D621070B5002603
:1061AC000C4D0D4C641BA410A64209D100F022F832
:1061BC0000260A4D0A4C641BA410A64205D170BDE2
:1061CC0055F8043B98470136EEE755F8043B9847E1
:1061DC000136F2E73C6200083C6200083C620008B1
:1061EC004062000803460244934200D1704703F812
:1061FC00011BF9E7F8B500BFF8BC08BC9E46704718
:0C620C00F8B500BFF8BC08BC9E46704707
:10621800000000000000000001020304060708094E
:1062280002030405060708090A0B0C0D0E0F1010CF
:04623800010200005F
:04623C0031510008D4
:046240000D510008F4
:0C6244000024F400100000000100000025
:04000005080053613B
:00000001FF
)";

uint8_t char_to_byte(char c)
{
    if(c >= '0' && c<= '9') return c - '0';
    if(c >= 'a' && c<= 'f') return c - 'a' + 10;
    if(c >= 'A' && c<= 'F') return c - 'A' + 10;
    return -1;
}

void convert_string_to_array_hex(char* input, uint8_t *output)
{
    uint8_t index =0;
    
    if(*input == ':')
    {
        input ++;
        while(*input != '\0')
        {
        uint8_t hex_value = char_to_byte(*input++)<<4;//4 bit cao
        hex_value |= char_to_byte(*input++);          //4 bit thap
        output[index++] = hex_value;

        //printf("%02x ",hex_value);
        }
    }
    //printf("\n");
}
uint8_t check_some(uint8_t *buff, uint8_t len)
{
    uint8_t some = 0;
    for(uint8_t i=0;i < len-1;i++){
        some += buff[i];
    } 
    some = ~some;
    some += 1;
    if(some == buff[len-1])
    {
        return status_ok;
    }
    return status_error;
}

void swap(uint8_t *a,uint8_t *b)
{
    uint8_t temp = *a;
    *a = *b;
    *b = temp;
}
void swap_4_byte(uint8_t *data, uint8_t len)
{
    for(int i=0; i<len; i+=4)
    {
      swap(&data[i+0],&data[i+3]);
      swap(&data[i+1],&data[i+2]);
    }
}

// void print_data(uint8_t *data,uint8_t len)
// {
//     for(uint8_t i=0;i<len;i++)
//     {
//         printf("%02X ",data[i]);
//     }
//     printf("\n");
// }

#endif