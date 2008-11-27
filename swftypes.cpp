#include "swftypes.h"
#include "tags.h"
#include <string.h>
RECT::RECT()
{
}

/*RECT::RECT(FILE* in)
{
	BitStream s(in);
	NBits=UB(5,s);
	Xmin=SB(NBits,s);
	Xmax=SB(NBits,s);
	Ymin=SB(NBits,s);
	Ymax=SB(NBits,s);
}*/

std::ostream& operator<<(std::ostream& s, const RECT& r)
{
	std::cout << '{' << (int)r.Xmin << ',' << r.Xmax << ',' << r.Ymin << ',' << r.Ymax << '}';
	return s;
}

std::ostream& operator<<(std::ostream& s, const RGB& r)
{
	std::cout << "RGB <" << (int)r.Red << ',' << (int)r.Green << ',' << (int)r.Blue << '>';
	return s;
}

void MATRIX::get4DMatrix(float matrix[16])
{
	memset(matrix,0,sizeof(float)*16);
	if(HasScale)
		matrix[0]=ScaleX;
	else
		matrix[0]=1;
	matrix[1]=RotateSkew0;

	matrix[4]=RotateSkew1;
	if(HasScale)
		matrix[5]=ScaleY;
	else
		matrix[5]=1;

	matrix[10]=1;

	matrix[12]=TranslateX;
	matrix[13]=TranslateY;
	matrix[15]=1;
}

std::ostream& operator<<(std::ostream& s, const MATRIX& r)
{
	float scaleX=1,scaleY=1;
	if(r.HasScale)
	{
		scaleX=r.ScaleX;
		scaleY=r.ScaleY;
	}
	std::cout << "| " << scaleX << ' ' << (int)r.RotateSkew0 << " |" << std::endl;
	std::cout << "| " << (int)r.RotateSkew1 << ' ' << scaleY << " |" << std::endl;
	std::cout << "| " << (int)r.TranslateX << ' ' << (int)r.TranslateY << " |" << std::endl;
	return s;
}

std::istream& operator>>(std::istream& stream, RECT& v)
{
	BitStream s(stream);
	v.NBits=UB(5,s);
	v.Xmin=SB(v.NBits,s);
	v.Xmax=SB(v.NBits,s);
	v.Ymin=SB(v.NBits,s);
	v.Ymax=SB(v.NBits,s);
	return stream;
}

std::istream& operator>>(std::istream& s, RGB& v)
{
	s >> v.Red >> v.Green >> v.Blue;
	return s;
}

std::istream& operator>>(std::istream& s, LINESTYLEARRAY& v)
{
	s >> v.LineStyleCount;
	if(v.LineStyleCount==0xff)
		throw "Line array extended not supported\n";
	std::cout << "Reading " << (int)v.LineStyleCount << " Line styles" << std::endl;
	v.LineStyles=new LINESTYLE[v.LineStyleCount];
	for(int i=0;i<v.LineStyleCount;i++)
	{
		s >> v.LineStyles[i];
	}
	return s;
}

std::istream& operator>>(std::istream& s, FILLSTYLEARRAY& v)
{
	s >> v.FillStyleCount;
	if(v.FillStyleCount==0xff)
		throw "Fill array extended not supported\n";
	std::cout << "Reading " << (int)v.FillStyleCount << " Fill styles" << std::endl;
	v.FillStyles=new FILLSTYLE[v.FillStyleCount];
	for(int i=0;i<v.FillStyleCount;i++)
	{
		s >> v.FillStyles[i];
	}
	return s;
}

std::istream& operator>>(std::istream& s, SHAPE& v)
{
	BitStream bs(s);
	v.NumFillBits=UB(4,bs);
	v.NumLineBits=UB(4,bs);
	v.ShapeRecords=SHAPERECORD(&v,bs);
	if(v.ShapeRecords.StateNewStyles+v.ShapeRecords.StateLineStyle+v.ShapeRecords.StateFillStyle1+v.ShapeRecords.StateFillStyle0+v.ShapeRecords.StateMoveTo)
	{
		SHAPERECORD* cur=&(v.ShapeRecords);
		while(1)
		{
			cur->next=new SHAPERECORD(&v,bs);
			cur=cur->next;
			if(cur->TypeFlag+cur->StateNewStyles+cur->StateLineStyle+cur->StateFillStyle1+cur->StateFillStyle0+cur->StateMoveTo==0)
				break;
		}
	}
}

std::istream& operator>>(std::istream& s, SHAPEWITHSTYLE& v)
{
	s >> v.FillStyles >> v.LineStyles;
	BitStream bs(s);
	v.NumFillBits=UB(4,bs);
	v.NumLineBits=UB(4,bs);
	v.ShapeRecords=SHAPERECORD(&v,bs);
	if(v.ShapeRecords.StateNewStyles+v.ShapeRecords.StateLineStyle+v.ShapeRecords.StateFillStyle1+v.ShapeRecords.StateFillStyle0+v.ShapeRecords.StateMoveTo)
	{
		SHAPERECORD* cur=&(v.ShapeRecords);
		while(1)
		{
			cur->next=new SHAPERECORD(&v,bs);
			cur=cur->next;
			if(cur->TypeFlag+cur->StateNewStyles+cur->StateLineStyle+cur->StateFillStyle1+cur->StateFillStyle0+cur->StateMoveTo==0)
				break;
		}
	}

	//s >> v.ShapeRecords;
}

std::istream& operator>>(std::istream& s, LINESTYLE& v)
{
	s >> v.Width >> v.Color;
	std::cout << "Line " << v.Width/20 << v.Color;
	return s;
}

std::istream& operator>>(std::istream& in, TEXTRECORD& v)
{
	BitStream bs(in);
	v.TextRecordType=UB(1,bs);
	v.StyleFlagsReserved=UB(3,bs);
	if(v.StyleFlagsReserved)
		throw "reserved";
	v.StyleFlagsHasFont=UB(1,bs);
	v.StyleFlagsHasColor=UB(1,bs);
	v.StyleFlagsHasYOffset=UB(1,bs);
	v.StyleFlagsHasXOffset=UB(1,bs);
	if(!v.TextRecordType)
		return in;
	if(v.StyleFlagsHasFont)
		in >> v.FontID;
	if(v.StyleFlagsHasColor)
	{
		RGB t;
		in >> t;
		v.TextColor=t;
	}
	if(v.StyleFlagsHasXOffset)
		in >> v.XOffset;
	if(v.StyleFlagsHasYOffset)
		in >> v.YOffset;
	if(v.StyleFlagsHasFont)
		in >> v.TextHeight;
	in >> v.GlyphCount;
	for(int i=0;i<v.GlyphCount;i++)
	{
		v.GlyphEntries.push_back(GLYPHENTRY(&v,bs));
		std::cout << "reading glyph " << i << "current size " << v.GlyphEntries.size() << std::endl;
	}

	return in;
}

std::istream& operator>>(std::istream& s, FILLSTYLE& v)
{
	s >> v.FillStyleType;
	if(v.FillStyleType!=0)
		throw "unsupported fill type";
	s >> v.Color;
	std::cout << "Fill color " << v.Color << std::endl;
	return s;
}

/*std::istream& operator>>(std::istream& in, GLYPHENTRY& v)
{
	BitStream bs(in);
	v.GlyphIndex = UB(v.parent->parent->GlyphBits,bs);
	v.GlyphAdvance = SB(v.parent->parent->AdvanceBits,bs);
	std::cout << "\tglyph " << v.GlyphAdvance << std::endl;
	return in;
}*/

GLYPHENTRY::GLYPHENTRY(TEXTRECORD* p,BitStream& bs):parent(p)
{
	GlyphIndex = UB(parent->parent->GlyphBits,bs);
	GlyphAdvance = SB(parent->parent->AdvanceBits,bs);
	std::cout << "\tglyph " << GlyphAdvance << std::endl;
}

SHAPERECORD::SHAPERECORD(SHAPE* p,BitStream& bs):parent(p),next(0)
{
	std::cout << "start shaperecord" << std::endl;
	TypeFlag = UB(1,bs);
	if(TypeFlag)
	{
		std::cout << "edge shaperecord" << std::endl;
		StraightFlag=UB(1,bs);
		NumBits=UB(4,bs);
		if(StraightFlag)
		{
			std::cout << "\tstraight line bits " << NumBits+2<< std::endl;

			GeneralLineFlag=UB(1,bs);
			if(!GeneralLineFlag)
				VertLineFlag=UB(1,bs);

			if(GeneralLineFlag || !VertLineFlag)
			{
				DeltaX=SB(NumBits+2,bs);
				std::cout << "DeltaX " << DeltaX << std::endl;
			}
			if(GeneralLineFlag || VertLineFlag)
			{
				DeltaY=SB(NumBits+2,bs);
				std::cout << "DeltaY " << DeltaY << std::endl;
			}
		}
		else
		{
			std::cout << "\tcurve line bits " << NumBits+2 << std::endl;
			
			ControlDeltaX=SB(NumBits+2,bs);
			ControlDeltaY=SB(NumBits+2,bs);
			AnchorDeltaX=SB(NumBits+2,bs);
			AnchorDeltaY=SB(NumBits+2,bs);
			
		}
	}
	else
	{
		std::cout << "non edge shaperecord" << std::endl;
		StateNewStyles = UB(1,bs);
		StateLineStyle = UB(1,bs);
		StateFillStyle1 = UB(1,bs);
		StateFillStyle0 = UB(1,bs);
		StateMoveTo = UB(1,bs);
		if(StateNewStyles)
		{
			throw "unsupported States in shaperecord";
		}
		if(StateMoveTo)
		{
			std::cout << "move to" << std::endl;
			MoveBits = UB(5,bs);
			std::cout <<"\tbits " << MoveBits << std::endl;
			MoveDeltaX = SB(MoveBits,bs);
			MoveDeltaY = SB(MoveBits,bs);
		}
		if(StateFillStyle0)
		{
			std::cout << "fill style 0 bits " << parent->NumFillBits << std::endl;
			FillStyle0=UB(parent->NumFillBits,bs);
		}
		if(StateFillStyle1)
		{
			std::cout << "fill style 1 bits " << parent->NumFillBits << std::endl;
			FillStyle1=UB(parent->NumFillBits,bs);
		}
		if(StateLineStyle)
		{
			std::cout << "line style bits " << parent->NumLineBits << std::endl;
			LineStyle=UB(parent->NumLineBits,bs);
		}
	}
}

std::istream& operator>>(std::istream& stream, CXFORMWITHALPHA& v)
{
	BitStream bs(stream);
	v.HasAddTerms=UB(1,bs);
	v.HasMultTerms=UB(1,bs);
	v.NBits=UB(4,bs);
	if(v.HasMultTerms)
	{
		v.RedMultTerm=SB(v.NBits,bs);
		v.GreenMultTerm=SB(v.NBits,bs);
		v.BlueMultTerm=SB(v.NBits,bs);
		v.AlphaMultTerm=SB(v.NBits,bs);
	}
	if(v.HasAddTerms)
	{
		v.RedAddTerm=SB(v.NBits,bs);
		v.GreenAddTerm=SB(v.NBits,bs);
		v.BlueAddTerm=SB(v.NBits,bs);
		v.AlphaAddTerm=SB(v.NBits,bs);
		std::cout << v.AlphaAddTerm << std::endl;
	}
}

std::istream& operator>>(std::istream& stream, MATRIX& v)
{
	BitStream bs(stream);
	v.HasScale=UB(1,bs);
	if(v.HasScale)
	{
		v.NScaleBits=UB(5,bs);
		v.ScaleX=FB(v.NScaleBits,bs);
		v.ScaleY=FB(v.NScaleBits,bs);
	}
	v.HasRotate=UB(1,bs);
	if(v.HasRotate)
	{
		throw "unsupported rotate";
	}
	v.NTranslateBits=UB(5,bs);
	v.TranslateX=SB(v.NTranslateBits,bs);
	v.TranslateY=SB(v.NTranslateBits,bs);
	return stream;
}
