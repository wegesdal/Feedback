/////////////////////////////////////////////////////////////////////////////
// Name:        minimal.cpp
// Purpose:     Minimal wxWidgets sample
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------
#include <filesystem>
// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------

// the application icon (under Windows it is in resources and even
// though we could still include the XPM here it would be unused)
#ifndef wxHAS_IMAGES_IN_RESOURCES
#include "/Users/wegesdal/Documents/wxWidgets-3.1.4/samples/sample.xpm"
#endif

#include "csv2.hpp"

#include "wx/grid.h"

#include <map>
#include <vector>
#include <string>

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// Define a new application type, each program should derive a class from wxApp
class MyApp : public wxApp
{
public:
    // override base class virtuals
    // ----------------------------
    
    // this one is called on application startup and is a good place for the app
    // initialization (doing it here and not in the ctor allows to have an error
    // return: if OnInit() returns false, the application terminates)
    virtual bool OnInit() wxOVERRIDE;
};

class MainFrame;

class BasicFrame : public wxFrame
{
public:
    BasicFrame( wxFrame * parent,
               const wxString& title,
               int xpos,
               int ypos,
               int width,
               int height
               );
    
    std::string fp;
    wxGrid * rubric_grid;
    wxGridCellAutoWrapStringRenderer * wrap;
    
    // hide
    void HideBasicFrame(wxCloseEvent& event);
    void OnSelectCell(wxGridEvent& ev);
    void Import(std::string path);
    void OnCellChanged(wxGridEvent& ev);
    
    
private:
    // any class wishing to process wxWidgets events must use this macro
    wxDECLARE_EVENT_TABLE();
};


// Define a new frame type: this is going to be our main frame
class MainFrame : public wxFrame
{
public:
    // ctor(s)
    MainFrame(const wxString& title,
              int xpos,
              int ypos,
              int width,
              int height
              );
    
    BasicFrame * rubric;
    BasicFrame * template_frame;
    BasicFrame * preview_frame;
    wxTextCtrl * control;
    wxTextCtrl * preview;
    
    std::map<std::string, std::vector<std::string>> symbol_map_vector;
    
    std::string filepath;
    wxGrid * grid;
    
    std::vector<std::string> scores_head;
    
    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnImportCSV(wxCommandEvent& event);
    void OnSaveCSV(wxCommandEvent& event);
    void OnSelectCell(wxGridEvent& ev);
    void AddCol(wxCommandEvent& event);
    void AddRow(wxCommandEvent& event);
    void RubricAddCol(wxCommandEvent& event);
    void RubricAddRow(wxCommandEvent& event);
    void DelCol(wxCommandEvent& event);
    void RubricDelCol(wxCommandEvent& event);
    void SetupTemplateFrame();
    void SetupPreviewFrame();
    void UpdatePreviewFrame();
    void OnTextChanged(wxCommandEvent& event);
    void ParseHeaders();
    
    
private:
    // any class wishing to process wxWidgets events must use this macro
    wxDECLARE_EVENT_TABLE();
};


// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items
    e_quit = wxID_EXIT,
    
    // it is important for the id corresponding to the "About" command to have
    // this standard value as otherwise it won't be handled properly under Mac
    // (where it is special and put into the "Apple" menu)
    e_about = wxID_ABOUT,
    e_open = wxID_OPEN,
    e_save = wxID_SAVE,
    e_delcol = wxID_FILE1,
    e_rubricdelcol = wxID_FILE2,
    e_addcol = wxID_FILE3,
    e_addrow = wxID_FILE4,
    e_rubricaddcol = wxID_FILE5,
    e_rubricaddrow = wxID_FILE6
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
EVT_MENU(e_quit,  MainFrame::OnQuit)
EVT_MENU(e_about, MainFrame::OnAbout)
EVT_MENU(e_open, MainFrame::OnImportCSV)
EVT_MENU(e_save, MainFrame::OnSaveCSV)
EVT_MENU(e_addcol, MainFrame::AddCol)
EVT_MENU(e_addrow, MainFrame::AddRow)
EVT_MENU(e_delcol, MainFrame::DelCol)
EVT_MENU(e_rubricdelcol, MainFrame::RubricDelCol)
EVT_MENU(e_rubricaddcol, MainFrame::RubricAddCol)
EVT_MENU(e_rubricaddrow, MainFrame::RubricAddRow)
EVT_GRID_SELECT_CELL(MainFrame::OnSelectCell)
EVT_TEXT(69, MainFrame::OnTextChanged)
wxEND_EVENT_TABLE()

wxBEGIN_EVENT_TABLE(BasicFrame, wxFrame)
EVT_CLOSE(BasicFrame::HideBasicFrame)
EVT_GRID_SELECT_CELL(BasicFrame::OnSelectCell)
EVT_GRID_CELL_CHANGED(BasicFrame::OnCellChanged)


wxEND_EVENT_TABLE()

// Create a new application object: this macro will allow wxWidgets to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also implements the accessor function
// wxGetApp() which will return the reference of the right type (i.e. MyApp and
// not wxApp)
wxIMPLEMENT_APP(MyApp);

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

// 'Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    std::cout<<"init"<<std::endl;
    // call the base class initialization method, currently it only parses a
    // few common command-line options but it could be do more in the future
    if ( !wxApp::OnInit() )
        return false;
    
    // create the main application window
    MainFrame * main = new MainFrame("Scores", 0, 0, 600, 600);
    
    
    // and show it (the frames, unlike simple controls, are not shown when
    // created initially)
    main->Show(true);
    
    
    SetTopWindow(main);
    
    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned false here, the
    // application would exit immediately.
    return true;
}

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

// constructor
BasicFrame::BasicFrame( wxFrame * parent, const wxString& title,
                       int xpos, int ypos, int width, int height )
: wxFrame( parent, -1, title, wxPoint(xpos, ypos), wxSize(width, height), wxFRAME_FLOAT_ON_PARENT | wxRESIZE_BORDER | wxMINIMIZE_BOX)
{
    rubric_grid = nullptr;
    wrap = new wxGridCellAutoWrapStringRenderer();
}

void BasicFrame::HideBasicFrame(wxCloseEvent &event)
{
    std::cout << "hiding child window";
    Hide();
}

void BasicFrame::Import(std::string path){
    fp = path;
    
    csv2::Reader<csv2::delimiter<','>,
    csv2::quote_character<'"'>,
    csv2::first_row_is_header<true>,
    csv2::trim_policy::trim_whitespace> csv;
    if (csv.mmap(path)) {
        const auto header = csv.header();
        std::vector<std::string> head;
        for (const auto cell: header) {
            std::string header_value;
            cell.read_value(header_value);
            head.push_back(header_value);
        }
        size_t rows{0}, cells{0};
        
        for (const auto row: csv) {
            // check for empty line at eof
            std::string value;
            row.read_raw_value(value);
            if (value.length()>0)
            {
                rows += 1;
            }
            for (const auto cell: row) {
                cells += 1;
            }
        }
        // Create a wxGrid object
        if (rubric_grid != nullptr)
        {
            delete rubric_grid; rubric_grid = nullptr;
        };
        
        if (wrap != nullptr) {
            delete wrap; wrap = nullptr;
        }
        
        wrap = new wxGridCellAutoWrapStringRenderer();
        
        rubric_grid = new wxGrid( this,
                                 -1,
                                 wxPoint( 0, 0 ),
                                 wxSize( 484, 322 ) );
        rubric_grid->CreateGrid( int(rows), int(cells/rows) );
        rubric_grid->SetDefaultColSize(400);
        rubric_grid->SetDefaultRowSize(100);
        rubric_grid->SetDefaultRenderer(wrap);
        
        for (int i = 0; i < int(cells/rows); i++) {
            rubric_grid->SetColLabelValue(i,head[i]);
        }
        wxBoxSizer *sizerMain=new wxBoxSizer(wxVERTICAL);
        sizerMain->Add(rubric_grid, 1, wxALL|wxEXPAND, 0);
        SetSizer(sizerMain);
        sizerMain->SetSizeHints(this);
        int y = 0;
        for (const auto row: csv) {
            int x = 0;
            for (const auto cell: row) {
                // Do something with cell value
                std::string value;
                
                cell.read_value(value);
                std::cout << value;
                rubric_grid->SetCellValue(y, x, value);
                x += 1;
            }
            y += 1;
        }
    }
}

// frame constructor
MainFrame::MainFrame(const wxString& title, int xpos, int ypos, int width, int height)
: wxFrame((wxFrame *) NULL, -1, title, wxPoint(xpos, ypos), wxSize(width, height), wxDEFAULT_FRAME_STYLE)

{
    Centre();
    rubric = new BasicFrame(this, "Rubric", 785, 22, 424, 400);
    
    template_frame = new BasicFrame(this, "Template", 785, 422, 424, 400);
    preview_frame = new BasicFrame(this, "Preview", 785, 822, 424, 400);
    SetupTemplateFrame();
    SetupPreviewFrame();
    
    grid = nullptr;
    
    // set the frame icon
    SetIcon(wxICON(sample));
    
#if wxUSE_MENUBAR
    // create a menu bar
    wxMenu *fileMenu = new wxMenu;
    wxMenu *rubricMenu = new wxMenu;
    wxMenu *scoresMenu = new wxMenu;
    
    // the "About" item should be in the help menu
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(e_about, "&About\tF1", "Show about dialog");
    
    fileMenu->Append(e_quit, "E&xit\tAlt-X", "Quit this program");
    fileMenu->Append(e_open, "Open Scores\tAlt-O", "Open CSV");
    fileMenu->Append(e_save, "Save Scores\tAlt-S", "Save CSV");
    
    scoresMenu->Append(e_addcol, "Add Column\tAlt-C");
    scoresMenu->Append(e_addrow, "Add Row\tAlt-R");
    scoresMenu->Append(e_delcol, "Delete Columns\tAlt-D");
    
    rubricMenu->Append(e_rubricaddcol, "Add Column\tAlt-Shift-C");
    rubricMenu->Append(e_rubricaddrow, "Add Row\tAlt-Shift-R");
    rubricMenu->Append(e_rubricdelcol, "Delete Columns\tAlt-Shift-D");
    
    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, "&File");
    menuBar->Append(helpMenu, "&Help");
    menuBar->Append(scoresMenu, "&Scores");
    menuBar->Append(rubricMenu, "&Rubric");
    
    
    
    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);
#else // !wxUSE_MENUBAR
    // If menus are not available add a button to access the about box
    wxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton* aboutBtn = new wxButton(this, wxID_ANY, "About...");
    aboutBtn->Bind(wxEVT_BUTTON, &MyFrame::OnAbout, this);
    sizer->Add(aboutBtn, wxSizerFlags().Center());
    SetSizer(sizer);
#endif // wxUSE_MENUBAR/!wxUSE_MENUBAR
    
#if wxUSE_STATUSBAR
    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar(2);
    SetStatusText("Welcome to Feedback!");
#endif // wxUSE_STATUSBAR
    
}
void BasicFrame::OnCellChanged(wxGridEvent &ev) {
    // iterate grid
    int colCount = rubric_grid->GetNumberCols();
    int rowCount = rubric_grid->GetNumberRows();
    std::ofstream stream(fp);
    csv2::Writer<csv2::delimiter<','>> writer(stream);
    std::vector<std::vector<std::string>> rows;
    // use column labels to generate new csv header
    
    std::vector<std::string> head;
    for (int i = 0; i < colCount;i++) {
        head.push_back(std::string(rubric_grid->GetColLabelValue(i)));
    }
    writer.write_row(head);
    for (int y = 0; y < rowCount; y++) {
        std::vector<std::string> row;
        for (int x = 0; x < colCount; x++) {
            auto value = std::string(rubric_grid->GetCellValue(y, x));
            if (value.length()>0) {
                row.push_back(value);
            } else {
                row.push_back(" ");
            }
        }
        writer.write_row(row);
    }
    stream.close();
}

void MainFrame::OnTextChanged(wxCommandEvent& event) {
    ParseHeaders();
    UpdatePreviewFrame();
}

void MainFrame::ParseHeaders() {
    for (auto h : scores_head) {
        if (h[0] == '@') {
            // load column into symbol_map_vector
            // need to deal with uppercase (make all lower?)
            std::cout << h << std::endl;
        } else if (h[0] == '#'){
            // parse rubric and load symbols into symbol_map_vector
            
        }
    }
}

void MainFrame::SetupTemplateFrame() {
    control = new wxTextCtrl(template_frame, 69, "@Name's favorite color is @color.", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_BESTWRAP);
//    std::ostream stream(control);
//    stream << "";
//    stream.flush();
}

void MainFrame::SetupPreviewFrame() {
    preview = new wxTextCtrl(preview_frame, -1, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_BESTWRAP);
    preview->SetEditable(false);
}

void MainFrame::UpdatePreviewFrame() {
    // TODO: parse headers and find/replace according to grid data
    preview->ChangeValue(control->GetValue());
}

// event handlers

void MainFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close
    Close(true);
}

void MainFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(wxString::Format
                 (
                  "Welcome to %s!\n"
                  "\n"
                  "This is the minimal wxWidgets sample\n"
                  "running under %s.",
                  wxVERSION_STRING,
                  wxGetOsDescription()
                  ),
                 "About wxWidgets minimal sample",
                 wxOK | wxICON_INFORMATION,
                 this);
}

void BasicFrame::OnSelectCell(wxGridEvent& ev) {
    ev.StopPropagation();
    ev.Skip();
}

void MainFrame::OnSelectCell(wxGridEvent& ev) {
    rubric->DestroyChildren();
    rubric->rubric_grid = nullptr;
    rubric->wrap = nullptr;
    std::cout<<grid->GetCellValue(ev.GetRow(), ev.GetCol())<<std::endl;
    wxString logBuf;
    //    if ( ev.Selecting() )
    //        logBuf << _T("Selected ");
    //    else
    //        logBuf << _T("Deselected ");
    //    logBuf << _T("cell at row ") << ev.GetRow()
    //    << _T(" col ") << ev.GetCol()
    //    << _T(" ( ControlDown: ")<< (ev.ControlDown() ? 'T':'F')
    //    << _T(", ShiftDown: ")<< (ev.ShiftDown() ? 'T':'F')
    //    << _T(", AltDown: ")<< (ev.AltDown() ? 'T':'F')
    //    << _T(", MetaDown: ")<< (ev.MetaDown() ? 'T':'F') << _T(" )");
    //
    //    //Indicate whether this column was moved
    //    if ( ((wxGrid *)ev.GetEventObject())->GetColPos( ev.GetCol() ) != ev.GetCol() )
    //        logBuf << _T(" *** Column moved, current position: ") << ((wxGrid *)ev.GetEventObject())->GetColPos( ev.GetCol() );
    
    //    wxLogMessage( wxT("%s"), logBuf.c_str() );
    
    
    rubric->SetTitle(scores_head[ev.GetCol()]);
    
    if (scores_head[ev.GetCol()][0]=='#') {
        std::cout<<"it's a rubric"<<std::endl;
        
        for(std::filesystem::path p : {filepath})
            if (std::filesystem::exists(p.remove_filename().append(scores_head[ev.GetCol()]+".csv"))) {
                rubric->Import(std::string(p.remove_filename().append(scores_head[ev.GetCol()]+".csv")));
            } else {
                std::cout<<"missing rubric"<<std::endl;
                logBuf << _T( "No Rubric found for ") << scores_head[ev.GetCol()] << _T(". Generating default...");
                wxLogMessage( wxT("%s"), logBuf.c_str() );
                std::ofstream stream(std::string(p.remove_filename().append(scores_head[ev.GetCol()]+".csv")));
                csv2::Writer<csv2::delimiter<','>> writer(stream);
                // @
                std::string new_header = scores_head[ev.GetCol()];
                new_header[0] = '@';
                writer.write_row(std::vector<std::string> {new_header});
                writer.write_row(std::vector<std::string> {"default text"});
                stream.close();
                
                rubric->Import(std::string(p.remove_filename().append(scores_head[ev.GetCol()]+".csv")));
            }
    }
    // you must call Skip() if you want the default processing
    // to occur in wxGrid
    ev.Skip();
    UpdatePreviewFrame();
}

void MainFrame::OnImportCSV(wxCommandEvent& WXUNUSED(event))

{
    auto origin = GetClientAreaOrigin();
    this->SetPosition(wxPoint(origin.x, origin.y+22));
    wxFileDialog
    openFileDialog(this, _("Open CSV file"), "", "",
                   "CSV files (*.csv)|*.csv", wxFD_OPEN|wxFD_FILE_MUST_EXIST);
    if (openFileDialog.ShowModal() == wxID_CANCEL)
        return;     // the user changed idea...
    
    
    filepath = std::string(openFileDialog.GetPath());
    for(std::filesystem::path p : {filepath})
        this->SetTitle(std::string(p.filename()));
    
    csv2::Reader<csv2::delimiter<','>,
    csv2::quote_character<'"'>,
    csv2::first_row_is_header<true>,
    csv2::trim_policy::trim_whitespace> csv;
    
    if (csv.mmap(filepath)) {
        
        const auto header = csv.header();
        scores_head.clear();
        for (const auto cell: header) {
            std::string header_value;
            cell.read_value(header_value);
            scores_head.push_back(header_value);
        }
        
        size_t rows{0}, cells{0};
        for (const auto row: csv) {
            // check for empty line at eof
            std::string value;
            row.read_raw_value(value);
            if (value.length()>0)
            {
                rows += 1;
            }
            for (const auto cell: row) {
                cells += 1;
            }
        }
        
        // Create a wxGrid object
        if (grid != nullptr) {
            delete grid; grid = nullptr;
        }
        grid = new wxGrid( this,
                          -1,
                          wxPoint( 0, 0 ),
                          wxSize( 600, 400 ) );
        
        grid->CreateGrid( int(rows), int(cells/rows) );
        for (int i = 0; i < int(cells/rows); i++) {
            grid->SetColLabelValue(i,scores_head[i]);
        }
        wxBoxSizer *sizerMain=new wxBoxSizer(wxVERTICAL);
        sizerMain->Add(grid, 1, wxALL|wxEXPAND, 0);
        SetSizer(sizerMain);
        sizerMain->SetSizeHints(this);
        
        int y = 0;
        for (const auto row: csv) {
            int x = 0;
            for (const auto cell: row) {
                // Do something with cell value
                std::string value;
                cell.read_value(value);
                grid->SetCellValue(y, x, value);
                x += 1;
            }
            y += 1;
        }
        rubric->Show(true);
        template_frame->Show(true);
        preview_frame->Show(true);
        //    // And set grid cell contents as strings
        //    grid->SetCellValue( 0, 0, "wxGrid is good" );
        //    // We can specify that some cells are read->only
        //    grid->SetCellValue( 0, 3, "This is read->only" );
        //    grid->SetReadOnly( 0, 3 );
        //    // Colours can be specified for grid cell contents
        //    grid->SetCellValue(3, 3, "green on grey");
        //    grid->SetCellTextColour(3, 3, *wxGREEN);
        //    grid->SetCellBackgroundColour(3, 3, *wxLIGHT_GREY);
        //    // We can specify the some cells will store numeric
        //    // values rather than strings. Here we set grid column 5
        //    // to hold floating point values displayed with width of 6
        //    // and precision of 2
        //    grid->SetColFormatFloat(5, 6, 2);
        //    grid->SetCellValue(0, 6, "3.1415");
        
    }
}

void MainFrame::AddCol(wxCommandEvent& event) {
    std::string value = std::string(wxGetTextFromUser("Enter new symbol or rubric title. eg: @example OR #example"));
    // TODO: CHECK IF SYMBOL UNIQUE
    // validate input
    if (value[0] == '@' || value[0] == '#') {
        grid->AppendCols(1, true);
        grid->SetColLabelValue(grid->GetNumberCols()-1, value);
        scores_head.push_back(value);
    } else {
        //dialog prompt
    }
}

void MainFrame::DelCol(wxCommandEvent& event) {
    if (grid != nullptr) {
        if (grid->GetSelectedCols().size() > 0) {
            grid->DeleteCols(grid->GetSelectedCols()[0], int(grid->GetSelectedCols().size()));
        }
    }
}

void MainFrame::AddRow(wxCommandEvent& WXUNUSED(event)) {
    grid->AppendRows(1, true);
}

void MainFrame::RubricAddCol(wxCommandEvent& event) {
    std::string value = std::string(wxGetTextFromUser("Enter new symbol. eg: @example"));
    // TODO: CHECK IF SYMBOL UNIQUE
    
    // validate input
    if (value[0] == '@') {
        rubric->rubric_grid->AppendCols(1, true);
        rubric->rubric_grid->SetColLabelValue(rubric->rubric_grid->GetNumberCols()-1, value);
    } else {
        // dialog prompt
    }
}

void MainFrame::RubricDelCol(wxCommandEvent& event) {
    if (rubric->rubric_grid != nullptr) {
        if (rubric->rubric_grid->GetSelectedCols().size() > 0) {
            rubric->rubric_grid->DeleteCols(rubric->rubric_grid->GetSelectedCols()[0], int(rubric->rubric_grid->GetSelectedCols().size()));
        }
    }
}

void MainFrame::RubricAddRow(wxCommandEvent& WXUNUSED(event)) {
    rubric->rubric_grid->AppendRows(1, true);
}

void MainFrame::OnSaveCSV(wxCommandEvent& WXUNUSED(event))
{
    // iterate grid
    grid->ResetColPos();
    int colCount = grid->GetNumberCols();
    int rowCount = grid->GetNumberRows();
    std::ofstream stream(filepath);
    csv2::Writer<csv2::delimiter<','>> writer(stream);
    std::vector<std::vector<std::string>> rows;
    std::vector<std::string> head;
    for (int i = 0; i < colCount;i++) {
        head.push_back(std::string(grid->GetColLabelValue(i)));
    }
    writer.write_row(head);
    for (int y = 0; y < rowCount; y++) {
        std::vector<std::string> row;
        for (int x = 0; x < colCount; x++) {
            auto value = std::string(grid->GetCellValue(y, x));
            if (value.length()>0) {
                row.push_back(value);
            } else {
                row.push_back(" ");
            }
        }
        writer.write_row(row);
        
    }
    stream.close();
}
