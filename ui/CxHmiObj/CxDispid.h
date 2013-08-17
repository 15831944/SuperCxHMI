////////////////////////////////////////////////////////////////////////////////////////////////
//CxComDispid.h : Common DISPID define file
//������Ŀ: AutoCx�е�ͼ�ζ���
//��������: 2001��11��
//�� �� ��: ¬�Ľ�
//��������: ͼ�ζ��������Խӿ�ID��
////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __CXCOMDISPID_H__
#define __CXCOMDISPID_H__

#define		CXDISPID_MIN						0x80010000
#define		CXDISPID_MAX						0x8001FFFF
#define		CXDISPID_PROP						CXDISPID_MIN //��չ������
#define		CXDISPID_METH						(CXDISPID_PROP + 0x4000) //�������Կ�ʼ 
#define		CXDISPID_EVEN						(CXDISPID_METH + 0x2000) //���󷽷���ʼ
#define		CXDISPID_EXTPROP					(CXDISPID_MIN + 0x8000) //�¼���ʼ


//��������DISPID
#define		CXPROPID_Name						(CXDISPID_PROP)			//����  
#define     CXPROPID_FillStyle					(CXDISPID_PROP + 1)			//�������        
#define     CXPROPID_HorFillDirection			(CXDISPID_PROP + 2)			//ˮƽ��䷽��    
#define     CXPROPID_VerFillDirection			(CXDISPID_PROP + 3)			//��ֱ��䷽��    
#define     CXPROPID_VerFillPercent				(CXDISPID_PROP + 4)			//��ֱ����      
#define     CXPROPID_HorFillPercent				(CXDISPID_PROP + 5)			//ˮƽ���� 
#define     CXPROPID_BrushImageFileName			(CXDISPID_PROP + 6)		//λͼ��ˢ��λͼ�ļ���
#define     CXPROPID_EdgeStyle					(CXDISPID_PROP + 7)			//�߽�����        
#define     CXPROPID_EdgeWidth					(CXDISPID_PROP + 8)			//�߽��߿�        
#define		CXPROPID_EdgeColor					(CXDISPID_PROP + 9)			//�߽���ɫ
#define		CXPROPID_HighLightEdgeColor			(CXDISPID_PROP + 10)			//�߽����ɫ  
#define		CXPROPID_FillColor					(CXDISPID_PROP + 11)			//�����ɫ  
#define		CXPROPID_HighLightFillColor			(CXDISPID_PROP + 12)			//������ɫ 
#define		CXPROPID_ForeColor					(CXDISPID_PROP + 13)   
#define		CXPROPID_BackColor					(CXDISPID_PROP + 14)			//������ɫ
#define		CXPROPID_BackStyle					(CXDISPID_PROP + 15)			//�������
#define		CXPROPID_HorPosition				(CXDISPID_PROP + 16)			//ˮƽλ��       
#define		CXPROPID_VerPosition				(CXDISPID_PROP + 17)			//��ֱλ��       
#define		CXPROPID_RotateAngle				(CXDISPID_PROP + 18)			//��ת�Ƕ�       
#define		CXPROPID_ScaleBasePoint				(CXDISPID_PROP + 19)			//�任����
#define		CXPROPID_UniformScale				(CXDISPID_PROP + 20)			//ͳһ����
#define		CXPROPID_HorScale					(CXDISPID_PROP + 21)			//ˮƽ���ű���
#define		CXPROPID_VerScale					(CXDISPID_PROP + 22)			//��ֱ���ű���
#define		CXPROPID_Height						(CXDISPID_PROP + 23)			//�߶�(DrawRectangle & DrawRoundRect & DrawEllipse & DrawLine)
#define		CXPROPID_Width						(CXDISPID_PROP + 24)			//���(DrawRectangle & DrawRoundRect & DrawEllipse & DrawLine)
#define		CXPROPID_StartX						(CXDISPID_PROP + 25)			//��������(DrawChord)
#define		CXPROPID_StartY						(CXDISPID_PROP + 26)			//���������(DrawChord)
#define		CXPROPID_EndX						(CXDISPID_PROP + 27)			//�յ������(DrawChord)
#define		CXPROPID_EndY						(CXDISPID_PROP + 28)			//�յ�������(DrawChord)
#define		CXPROPID_MiddleX					(CXDISPID_PROP + 29)			//�м�������(DrawChord)
#define		CXPROPID_MiddleY					(CXDISPID_PROP + 30)			//�м��������(DrawChord)
#define		CXPROPID_CenterX					(CXDISPID_PROP + 31)			//���ĵ������(DrawPie)
#define		CXPROPID_CenterY					(CXDISPID_PROP + 32)			//���ĵ�������(DrawPie)
#define		CXPROPID_PieType					(CXDISPID_PROP + 33)			//������ʽ(DrawPie)
#define		CXPROPID_StartAngle					(CXDISPID_PROP + 34)			//԰����ʼ�Ƕ�(DrawPie)
#define		CXPROPID_SweepAngle					(CXDISPID_PROP + 35)			//԰����ֹ�Ƕ�(DrawPie)
#define		CXPROPID_Radius						(CXDISPID_PROP + 36)			//԰���뾶(DrawPie)
#define		CXPROPID_LineJoinStyle				(CXDISPID_PROP + 37)			//�߽�ͷ����(DrawPolyLine & DrawLine)
#define		CXPROPID_Caption					(CXDISPID_PROP + 38)			//�ı�(DrawText)
#define		CXPROPID_Font						(CXDISPID_PROP + 39)
#define		CXPROPID_FontName					(CXDISPID_PROP + 40)			//������(DrawText)
#define		CXPROPID_FontSize					(CXDISPID_PROP + 41)
#define		CXPROPID_FontStyle					(CXDISPID_PROP + 42)
#define		CXPROPID_UnderLine					(CXDISPID_PROP + 43)
#define		CXPROPID_StrikeThrough				(CXDISPID_PROP + 44)
#define		CXPROPID_Alignment					(CXDISPID_PROP + 45)
#define		CXPROPID_FontColor					(CXDISPID_PROP + 46) 
#define		CXPROPID_AutoSize					(CXDISPID_PROP + 47)			//�Զ��ߴ�(DrawText)
#define		CXPROPID_Visible					(CXDISPID_PROP + 48)			//�ɼ�(All)
#define		CXPROPID_Layer						(CXDISPID_PROP + 49)			//��(All)
#define		CXPROPID_RoateBasePointX			(CXDISPID_PROP + 50)
#define		CXPROPID_RoateBasePointY			(CXDISPID_PROP + 51)
#define		CXPROPID_ToolTipText				(CXDISPID_PROP + 52)			//��ʾ�ı�
#define		CXPROPID_MousePointer				(CXDISPID_PROP + 53)			//���ָ��
#define		CXPROPID_SecurityArea				(CXDISPID_PROP + 54)			//��ȫ��
#define		CXPROPID_Flow						(CXDISPID_PROP + 55)		
#define		CXPROPID_FlowDirection				(CXDISPID_PROP + 56)			
#define		CXPROPID_FlowSpeed					(CXDISPID_PROP + 57)
#define		CXPROPID_LiquidColor				(CXDISPID_PROP + 58) 
#define		CXPROPID_ScanInterval				(CXDISPID_PROP + 59) 
#define		CXPROPID_GridEnabled				(CXDISPID_PROP + 60)
#define		CXPROPID_GridColor  				(CXDISPID_PROP + 61) 
#define		CXPROPID_VisibleLayer				(CXDISPID_PROP + 62)  
#define		CXPROPID_TagGroup					(CXDISPID_PROP + 63)
#define		CXPROPID_AssocTagGroup				(CXDISPID_PROP + 64)
#define		CXPROPID_Description				(CXDISPID_PROP + 65)
#define		CXPROPID_Privilege					(CXDISPID_PROP + 66)			//Ȩ��
#define		CXPROPID_LeftMargin					(CXDISPID_PROP + 67)
#define		CXPROPID_TopMargin					(CXDISPID_PROP + 68)
#define		CXPROPID_RightMargin				(CXDISPID_PROP + 69)
#define		CXPROPID_BottomMargin				(CXDISPID_PROP + 70)
#define		CXPROPID_RecordSet					(CXDISPID_PROP + 71)
#define		CXPROPID_KeepTogether				(CXDISPID_PROP + 72)
#define		CXPROPID_ForcePageBreak 			(CXDISPID_PROP + 73)
#define		CXPROPID_DataConnection				(CXDISPID_PROP + 74)
#define		CXPROPID_DataRecordset				(CXDISPID_PROP + 75)
#define		CXPROPID_PageCount					(CXDISPID_PROP + 76)
#define		CXPROPID_Page						(CXDISPID_PROP + 77)
#define		CXPROPID_BorderStyle				(CXDISPID_PROP + 78)
#define		CXPROPID_StartUpPosition			(CXDISPID_PROP + 79)
//#define		CXPROPID_ShowModal					(CXDISPID_PROP + 80)
#define		CXPROPID_Value						0
#define		CXPROPID_VariableType				(CXDISPID_PROP + 81)
#define		CXPROPID_Transparency				(CXDISPID_PROP + 82)
#define		CXPROPID_Enabled					(CXDISPID_PROP + 83)
#define		CXPROPID_Locked						(CXDISPID_PROP + 84)
#define		CXPROPID_Menu						(CXDISPID_PROP + 85)
#define		CXPROPID_TabIndex					(CXDISPID_PROP + 86)
#define		CXPROPID_TabStop					(CXDISPID_PROP + 87)
#define		CXPROPID_Default					(CXDISPID_PROP + 88)
#define		CXPROPID_Cancel						(CXDISPID_PROP + 89)
#define		CXPROPID_Objects					(CXDISPID_PROP + 90)
#define		CXPROPID_WordWrap					(CXDISPID_PROP + 91)
#define		CXPROPID_IdealHeight				(CXDISPID_PROP + 92)
#define		CXPROPID_ControlBox					(CXDISPID_PROP + 93)
#define		CXPROPID_WindowHandle				(CXDISPID_PROP + 94)
#define		CXPROPID_AllowDirectDraw			(CXDISPID_PROP + 95)

#define		CXMETHID_IsConnected				(CXDISPID_METH)
#define		CXMETHID_ConnectObject				(CXDISPID_METH + 1)
#define		CXMETHID_ConnectDirect				(CXDISPID_METH + 2)		
#define		CXMETHID_GetConnectObject			(CXDISPID_METH + 3)		
#define		CXMETHID_Disconnect					(CXDISPID_METH + 4)			
#define		CXMETHID_GetConnectState			(CXDISPID_METH + 5)
#define		CXMETHID_AddPoint					(CXDISPID_METH + 6)		
#define		CXMETHID_GetNumberOfPoints			(CXDISPID_METH + 7)		
#define		CXMETHID_DeletePoint				(CXDISPID_METH + 8)			
#define		CXMETHID_GetPointAt					(CXDISPID_METH + 9)
#define		CXMETHID_SetFocus					(CXDISPID_METH + 10)
#define		CXMETHID_GetHitObject				(CXDISPID_METH + 11)
#define		CXMETHID_GetObjectByName			(CXDISPID_METH + 12)
#define		CXMETHID_ForceRefresh				(CXDISPID_METH + 13)
#define		CXMETHID_BindDataLinksToTagGroup	(CXDISPID_METH + 14)

#define		CXEVENID_CLICK						(CXDISPID_EVEN) 
#define		CXEVENID_DBLCLICK					(CXDISPID_EVEN + 1) 
#define		CXEVENID_KEYDOWN					(CXDISPID_EVEN + 2)   
#define		CXEVENID_KEYUP						(CXDISPID_EVEN + 3)   
#define		CXEVENID_KEYPRESS					(CXDISPID_EVEN + 4)   
#define		CXEVENID_MOUSEDOWN					(CXDISPID_EVEN + 5)   
#define		CXEVENID_MOUSEMOVE					(CXDISPID_EVEN + 6)   
#define		CXEVENID_MOUSEUP					(CXDISPID_EVEN + 7)
#define		CXEVENID_INITIALIZE					(CXDISPID_EVEN + 8) 
#define		CXEVENID_ACTIVATE					(CXDISPID_EVEN + 9)   
#define		CXEVENID_DEACTIVATE					(CXDISPID_EVEN + 10)  
#define		CXEVENID_SCAN						(CXDISPID_EVEN + 11)
#define		CXEVENID_MOUSELEAVE					(CXDISPID_EVEN + 12)
#define		CXEVENID_TERMINATE					(CXDISPID_EVEN + 13)
#define		CXEVENID_RESIZE						(CXDISPID_EVEN + 14)
#define		CXEVENID_QUERYCLOSE					(CXDISPID_EVEN + 15)

#define		CXPROPID_DEFAULT_EDGECOLOR			0x80020000
#define		CXPROPID_DEFAULT_FILLCOLOR			0x80020001
#define		CXPROPID_DEFAULT_FILLSTYLE			0x80020002
#define		CXPROPID_DEFAULT_FONTCOLOR			0x80020003

#endif //__CXCOMDISPID_H__