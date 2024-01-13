QT_DEBUGGER_SRC = ../Strategy/qtDebugger

all: $(QT_DEBUGGER_SRC)/main_widget.moc.cpp $(QT_DEBUGGER_SRC)/render_area.moc.cpp 

$(QT_DEBUGGER_SRC)/main_widget.moc.cpp: $(QT_DEBUGGER_SRC)/main_widget.h
	moc $(QT_DEBUGGER_SRC)/main_widget.h -o $(QT_DEBUGGER_SRC)/main_widget.moc.cpp

$(QT_DEBUGGER_SRC)/render_area.moc.cpp: $(QT_DEBUGGER_SRC)/render_area.h
	moc $(QT_DEBUGGER_SRC)/render_area.h -o $(QT_DEBUGGER_SRC)/render_area.moc.cpp

clean:
	rm $(QT_DEBUGGER_SRC)/*.moc.cpp

