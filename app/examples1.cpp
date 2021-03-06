#include "examples1.hpp"
#include "util.hpp"

void
Lesson01_CreateBookAndSetColumnWidths()
{
	ods::Book book;
	auto *sheet = book.CreateSheet("Sheet1");
	auto *row = sheet->CreateRow(0); // create first/top row

	// Columns, cells and rows count is zero-based, e.g. 0 = 1, 4 = 5.
	// Cells and rows must always be created in ascending order.

	row->CreateCell(1)->SetValue("Index 1, cell 2");
	row->CreateCell(6)->SetValue("Index 6, cell 7");
	
	// Columns which don't have their widths set explicitly
	// will default to the default width.
	// Always set column widths in ascending order, CORRECT:
	sheet->SetColumnWidth(0, "5.0cm"); // col 1
	sheet->SetColumnWidth(2, "0.5in"); // col 3
	sheet->SetColumnWidth(7, "3.92in"); // col 8
	
	// INCORRECT:
	// sheet->SetColumnWidth(0, "5.0cm"); // col 1
	// sheet->SetColumnWidth(7, "3.92in"); // col 8
	// sheet->SetColumnWidth(2, "0.5in"); // col 3

	Save(book);
}

void
Lesson02_CreateCellsOfDifferentTypes()
{
	ods::Book book;
	auto *sheet = book.CreateSheet("Sheet1");
	auto *row = sheet->CreateRow(0);

	auto *cell = row->CreateCell(0);
	// will hold a string value
	cell->SetValue("string");

	cell = row->CreateCell(1);
	// will hold a float value
	// Note: internally the API stores/uses numbers as doubles, but the
	// ODF standard seems to use/store as floats
	cell->SetValue(3.14);

	// Cell with percentage value
	cell = row->CreateCell(2);
	cell->SetPercentageValue(0.8, 2); // 0.8 = 80.00%, second param (2) is
	// the number of leading zeroes after the dot, in this case
	// it's ".00" from "80.00%"

	// Cell with another percentage value
	cell = row->CreateCell(3);
	cell->SetPercentageValue(17.2); // 17.2 = 1720%, the second param
	// by default is zero, hence it will show up as "1720%" and not
	// as "1720.0%" or "1720.00%".

	Save(book);
}

void
Lesson03_UseFontsAndStyles()
{
	/** Note: when applying same style to many cells - don't create a new
	style for each cell, instead use the same style for them. For example:

	WRONG:
	for(int i=0; i<10; i++)
	{
		auto *style = book.CreateCellStyle();
		style->SetBackground(QColor(200, 0, 0));
		auto *cell = row->CreateCell(i);
		cell->SetStyle(style);
	}

	RIGHT:
	auto *style = book.CreateCellStyle();
	style->SetBackground(QColor(200, 0, 0));
	for(int i=0; i<10; i++)
	{
		auto *cell = row->CreateCell(i);
		cell->SetStyle(style);
	}
	**/

	ods::Book book;
	auto *sheet = book.CreateSheet("name");
	auto *row = sheet->CreateRow(0);

	auto *cell = row->CreateCell(0);
	cell->SetValue("string");

	// Note: styles are managed by ods::Book, so never delete styles yourself.
	auto *style = book.CreateCellStyle();
	style->SetFontName("Verdana");
	style->SetFontSize(20.0);
	style->SetTextColor(QColor(200, 0, 0));
	cell->SetStyle(style);

	// We got a big font size in a cell, so when done creating all the cells
	// of the row tell the row to resize itself vertically for the contents
	// of the cell with the biggest font size to be fully visible by default
	// when opened with an office suite like LibreOffice or MS Office.
	row->SetOptimalHeightStyle();

	cell = row->CreateCell(1);
	//derive a new style from parent style and set yellow background
	style = style->Derive();
	style->SetBackgroundColor(QColor(255, 255, 0));
	cell->SetValue(3.14f);
	cell->SetStyle(style);

	Save(book);
}

void
Lesson04_Alignment()
{
	ods::Book book;
	auto *sheet = book.CreateSheet("name");
	auto *row = sheet->CreateRow(0);

	auto *cell = row->CreateCell(0);
	cell->SetValue("string");
	auto *style = book.CreateCellStyle();
	style->SetHAlignment(ods::HAlign::Center);
	//aligns the string value "string" of the cell to the center
	cell->SetStyle(style);

	cell = row->CreateCell(1);
	cell->SetValue(3.14);
	style = book.CreateCellStyle();
	//aligns the number value "3.14" of the cell to the right
	style->SetHAlignment(ods::HAlign::Right);
	cell->SetStyle(style);

	Save(book);
}

void
Lesson05_Borders()
{
	// Borders are used by creating a border class and then applying it
	// to a style which then gets applied to the cell

	ods::Book book;
	auto *sheet = book.CreateSheet("name");
	auto *row = sheet->CreateRow(1);

	auto *cell = row->CreateCell(1);
	cell->SetValue("string");

	auto *style = book.CreateCellStyle();

	auto *base_border = new ods::style::Border();// thin solid black by default
	base_border->SetColor(QColor(0, 0, 255)); // make the border blue
	base_border->SetStyle(ods::BorderStyle::Solid);
	base_border->SetWidth(ods::BorderWidth::Medium);
	style->SetBorder(base_border);
	// cell which contains "string" will have solid blue border all around it
	cell->SetStyle(style);

	cell = row->CreateCell(5);
	cell->SetValue(3.14);

	auto *top_border = base_border->Derive();
	top_border->sides_set(ods::BorderSideTop);
	top_border->SetColor(QColor(0, 255, 0)); // (top) green border
	top_border->SetWidth("0.05in");

	style = book.CreateCellStyle();
	style->SetBorder(top_border);
	// cell which contains "3.14" will only have a (very) thick top green border
	cell->SetStyle(style);

	Save(book);
}

void
Lesson06_CellSpan()
{
	ods::Book book;
	auto *sheet = book.CreateSheet("name");
	auto *row = sheet->CreateRow(0);

	auto *cell = row->CreateCell(0);
	cell->SetValue("string");
	// By default a cell spans over 1 row and 1 column.
	// Make the cell span over 2 rows and 4 columns:
	cell->SetRowColSpan(2, 4);
	
	Save(book);
}

void
Lesson07_UsingImages()
{
	// Images are set on the sheet relative to a cell

	ods::Book book;
	auto *sheet = book.CreateSheet("Sheet1");
	auto *row = sheet->CreateRow(3);
	auto *cell = row->CreateCell(2);

	//let's load a .png image "image.png" from the home dir into this cell
	QFile file(QDir::home().filePath("image.png"));
	ods::DrawFrame *draw_frame = cell->CreateDrawFrame(file);
	if (draw_frame == nullptr)
	{
		qDebug() << "DrawFrame == nullptr";
		return;
	}
	// optional: resize it
	draw_frame->SetSize(QSize(40, 80));
	// optional: move the image by x,y points relative to the cell
	draw_frame->SetOffset(QPoint(60, 0));

	Save(book);
}

void
Lesson08_ReadFormula()
{
	auto path = QDir::home().filePath("ReadFormula.ods");
	QFile file(path);
	if (!file.exists())
	{
		qDebug() << "No such file:" << path;
		return;
	}
	ods::Book book(path);
	auto *sheet = book.sheet(0);

	if (sheet == nullptr)
	{
		qDebug() << "No sheet at 0";
		return;
	}
	const int kRow = 0;
	auto *row = sheet->row(kRow);

	if (row == nullptr)
	{
		qDebug() << "No row at " << kRow;
		return;
	}
	const int kCol = 1;
	auto *cell = row->cell(kCol);

	if (cell == nullptr)
	{
		qDebug() << "No cell at col" << kCol;
		return;
	}

	if (!cell->HasFormula())
	{
		qDebug() << "Cell has no formula";
		return;
	}
	
	auto *formula = cell->formula();
	if (formula->HasAnError())
	{
		qDebug() << "Formula not supported, error:" << formula->error();
		return;
	}

	ods::Value &value = formula->value();
	if (value.IsNotSet())
	{
		qDebug() << "Formula value is not set";
		return;
	}
	if (value.IsDouble())
	{
		double *num = value.AsDouble();
		qDebug() << "Formula value as a double:" << QString::number(*num);
	} else if (value.IsString()) {
		QString *str = value.AsString();
		qDebug() << "Formula value as a string:" << *str;
	} else if (value.IsDate()) {
		QDateTime *dt = value.AsDate();
		qDebug() << "Formula value as a date/time:" << dt->toString();
	} else {
		qDebug() << "Formula value type not supported";
	}
}

void
Lesson09_WriteOneSheetFormula()
{
	// Only very basic formula functionality is supported.
	// In this example all cells the formula depends on are on the same sheet,
	// See Lesson10_WriteCrossSheetFormula() for an example with two sheets.

	ods::Book book;
	auto *sheet = book.CreateSheet("Sheet1");

	for (int i = 2; i < 10; i++)
	{
		auto *row = sheet->CreateRow(i);

		auto *cell1 = row->CreateCell(1);
		cell1->SetValue(15);

		auto *cell2 = row->CreateCell(2);
		cell2->SetValue(i);

		// Create a formula that adds the numbers from cell1 and cell2,
		// multiplies the resulting sum by two and applies the
		// resulting formula to cell3. Schematically the formula looks
		// like this: cell3 = "(cell1 + cell2) * 2"
		auto *formula = new ods::Formula();
		formula->Add(ods::Grouping::Open);
		formula->Add(cell1);
		formula->Add(ods::Op::Add);
		formula->Add(cell2);
		formula->Add(ods::Grouping::Close);
		formula->Add(ods::Op::Mult);
		formula->Add(2);

		auto *cell3 = row->CreateCell(3);
		cell3->SetFormula(formula);
	}

	Save(book);
}

void
Lesson10_WriteCrossSheetFormula()
{
	// Create a cross-sheet formula like this:
	// sheet1.cell2 = sheet1.cell1 + sheet2.cell1

	ods::Book book;
	auto *sheet1 = book.CreateSheet("Sheet1");
	auto *sheet2 = book.CreateSheet("Sheet2");

	auto *sheet1_row = sheet1->CreateRow(0);
	auto *sheet1_cell1 = sheet1_row->CreateCell(0);
	sheet1_cell1->SetValue(15);

	auto *sheet2_cell1 = sheet2->CreateRow(0)->CreateCell(0);
	sheet2_cell1->SetValue(5);

	auto *formula = new ods::Formula();

	// Since the 2nd param (the sheet) is not specified it defaults to the same
	// sheet the formula cell is on.
	formula->Add(sheet1_cell1);
	formula->Add(ods::Op::Add);

	// The second param "sheet2" tells the API the cell is on a different sheet
	formula->Add(sheet2_cell1, sheet2);

	auto *formula_cell = sheet1_row->CreateCell(1);
	formula_cell->SetFormula(formula);

	Save(book);
}

void
PrivateTest()
{
	auto path = QDir::home().filePath("text-span-example.ods");
	QFile file(path);
	if (!file.exists())
	{
		qDebug() << "No such file:" << path;
		return;
	}
	ods::Book book(path);
	auto *sheet = book.sheet(0);

	if (sheet == nullptr)
	{
		qDebug() << "No sheet at 0";
		return;
	}
	const int kRow = 1;
	auto *row = sheet->row(kRow);

	if (row == nullptr)
	{
		qDebug() << "No row at " << kRow;
		return;
	}
	const int kCol = 0;
	auto *cell = row->cell(kCol);

	if (cell == nullptr)
	{
		qDebug() << "No cell at col" << kCol;
		return;
	}
	
	auto &value = cell->value();
	if (value.IsString())
	{
		qDebug() << "value as string:" << *value.AsString();
	} else {
		qDebug() << "value not a string";
	}
	
	const int kRow2 = 0;
	row = sheet->row(kRow2);

	if (row == nullptr)
	{
		qDebug() << "No row at " << kRow2;
		return;
	}
	const int kCol2 = 1;
	cell = row->cell(kCol2);

	if (cell == nullptr)
	{
		qDebug() << "No cell at col" << kCol2;
		return;
	}
	
	auto &value2 = cell->value();
	if (value2.IsString())
	{
		qDebug() << "value as string:" << *value2.AsString();
	} else {
		qDebug() << "value not a string";
	}
}

