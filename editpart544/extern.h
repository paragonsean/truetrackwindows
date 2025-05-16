extern HINSTANCE MainInstance;

extern HWND      MainWindow;
extern HWND      MainDialogWindow;
extern HWND      ParamDialogWindow;

#ifdef _MACHINE_CLASS_
extern MACHINE_CLASS   CurrentMachine;
#endif

#ifdef _PART_CLASS_
extern PART_CLASS      CurrentPart;
#endif

#ifdef _VISI_PARAM_H_
extern PARAMETER_CLASS CurrentParam;
#endif

#ifdef _FTANALOG_CLASS_
extern FTANALOG_CLASS  CurrentFtAnalog;
#endif

#ifdef _GENLIST_H
extern GENERIC_LIST_CLASS MachineList;
#endif

#ifdef _SURETRAK_PARAM_CLASS_
extern SURETRAK_PARAM_CLASS    SureTrakParam;
#endif