#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    Install();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::Install()
{
    FWPM_SESSION0   Session;
    HANDLE          hEngine;

    RtlZeroMemory(&Session, sizeof(Session));
    Session.flags = FWPM_SESSION_FLAG_DYNAMIC;
    Session.displayData.name = const_cast<wchar_t*>(L"MyWfpSession");
    Session.displayData.description = const_cast<wchar_t*>(L"My WFP Session");

    DWORD Status = FwpmEngineOpen0(
        NULL,
        RPC_C_AUTHN_DEFAULT,
        NULL,
        &Session,
        &hEngine
    );

    if (ERROR_SUCCESS != Status)
    {
        cout << "FwpmEngineOpen0 failed with status" << hex<< Status;
        return;
    }

    Status = FwpmTransactionBegin0(hEngine, NULL);
    if (ERROR_SUCCESS != Status)
    {
        cout << "FwpmTransactionBegin0 failed with status " << hex << Status;
        FwpmTransactionAbort0(hEngine);
        goto END;
    }

    FWPM_SUBLAYER0 Sublayer;
    RtlZeroMemory(&Sublayer, sizeof(Sublayer));

    Sublayer.subLayerKey = GUID_SUBLAYER;
    Sublayer.displayData.name = const_cast <wchar_t*>(L"MyWfpSublayer");
    Sublayer.displayData.description = const_cast <wchar_t*>(L"My WFP Sublayer");
    Sublayer.weight = 0x100;

    Status = FwpmSubLayerAdd0(hEngine, &Sublayer, NULL);

    if (ERROR_SUCCESS != Status)
    {
        cout << "FwpmSublayerAdd0 failed with status " << hex<< Status;
        goto END;
    }

    FWPM_CALLOUT0 Callout;
    RtlZeroMemory(&Callout, sizeof(Callout));

    Callout.calloutKey = GUID_CALLOUT;
    Callout.applicableLayer = FWPM_LAYER_ALE_AUTH_CONNECT_V4;
    Callout.displayData.name = const_cast <wchar_t*>(L"MyWfpConnectCallout");
    Callout.displayData.description = const_cast <wchar_t*>(L"My WFP Connection Callout");

    UINT32 CalloutId;
    Status = FwpmCalloutAdd0(hEngine, &Callout, NULL, &CalloutId);

    if (ERROR_SUCCESS != Status)
    {
        cout << "FwpmCalloutAdd0 failed with status "<< hex << Status;
        goto END;
    }

    cout << CalloutId;

    UINT32 const NumConds = 1;
    FWPM_FILTER_CONDITION0 Conds[NumConds];
    Conds[0].fieldKey               = FWPM_CONDITION_IP_PROTOCOL;
    Conds[0].matchType              = FWP_MATCH_EQUAL;
    Conds[0].conditionValue.type    = FWP_UINT8;
    Conds[0].conditionValue.uint8   = IPPROTO_TCP;

    FWPM_FILTER0 Filter;
    RtlZeroMemory(&Filter, sizeof (Filter));

    Filter.providerKey = NULL;
    Filter.filterKey = GUID_FILTER;
    Filter.subLayerKey = GUID_SUBLAYER;
    Filter.action.calloutKey = GUID_CALLOUT;
    Filter.displayData.name        = const_cast <wchar_t*>(L"MyWfpConnectFilter");
    Filter.displayData.description = const_cast <wchar_t*>(L"My WFP Connection Filter");
    Filter.layerKey                = FWPM_LAYER_ALE_AUTH_CONNECT_V4;
    Filter.weight.type             = FWP_UINT8;
    Filter.numFilterConditions     = NumConds;
    Filter.filterCondition         = &Conds[0];
    Filter.action.type             = FWP_ACTION_BLOCK;
    Filter.weight.uint8 = 1;

    UINT64 FilterId;

    Status = FwpmFilterAdd0(hEngine, &Filter, NULL, &FilterId);

    if (ERROR_SUCCESS != Status)
    {
        cout << "FwpmFilterAdd0 failed with status" << hex <<  Status;
        goto END;
    }

    cout << FilterId;

    Status = FwpmTransactionCommit0(hEngine);
    if (ERROR_SUCCESS != Status)
    {
        cout << "FwpmTransactionCommit0 failed with status"<< hex << Status;
        goto END;
    }
    QThread::sleep(20);
END0:
    FwpmEngineClose0(hEngine);
    return;
END:
    FwpmTransactionAbort0(hEngine);
    goto END0;
}
