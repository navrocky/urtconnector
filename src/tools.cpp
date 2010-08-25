
#include <algorithm>
#include <vector>
#include <stdexcept>

#include <boost/bind.hpp>
#include <boost/assign/std/map.hpp>

#include <QAction>
#include <QString>
#include <QWidget>
#include <QLabel>
#include <QEvent>
#include <QDockWidget>
#include <QStyle>

#include "tools.h"

using namespace std;

QAction* add_separator_action(QWidget* w, const QString& text)
{
    QAction* separator = new QAction(w);
    separator->setSeparator(true);
    separator->setText(text);
    w->addAction(separator);
    
    return separator;
}


QColor color(const QString & str)
{
    static QMap<int, QColor> color_map;

    if( color_map.isEmpty() )
    {
        color_map[0] = QColor(Qt::black);
        color_map[1] = QColor(Qt::red);
        color_map[2] = QColor(Qt::green);
        color_map[3] = QColor(Qt::yellow);
        color_map[4] = QColor(Qt::blue);
        color_map[5] = QColor(Qt::cyan);
        color_map[6] = QColor(Qt::magenta);
        color_map[7] = QColor(Qt::white);
    }

    bool ok;
    int col_num = str.toInt(&ok) % 8;
    if (!ok)
        col_num = 7;
    
    return color_map[col_num].lighter();
}

QString colorize(const QString& str, const QColor& color)
{
    static QString colored("<font color=\"%1\">%2</font>");
    return colored.arg( color.name(), str );
}


QString make_css_colored(QString str, const QString& skip)
{
    static QStringList markers;
    if ( markers.isEmpty() ) markers <<"1"<<"2"<<"3"<<"4"<<"5"<<"6"<<"7";

    for( QStringList::iterator it = markers.begin(); it != markers.end(); ++it )
    {
        if( str.startsWith( *it ) )
        {
            str.remove(0,1);
            if (*it != skip ) return colorize( str, color(*it) );
        }
    }
    
    return str;
}

QString q3coloring(const QString & str, const QString& skip)
{
    //split incoming string by quake3 color-markers
    QStringList lst = str.split( QRegExp("\\^") );
    //removing empty lines
    lst.erase( std::remove_if( lst.begin(), lst.end(), bind( &QString::isEmpty, _1 ) ), lst.end() );
    //replacing quake3 color-markers by html-formated text
    std::transform(lst.begin(), lst.end(), lst.begin(), boost::bind(make_css_colored, _1, skip) );
    //joining list to plain string
    return lst.join("");
}

QColor choose_for_background(Qt::GlobalColor standard, const QColor& background)
{
    static std::map<Qt::GlobalColor, QColor> light_colors;
    static std::map<Qt::GlobalColor, QColor> dark_colors;

    if( light_colors.empty() )
    {
        boost::assign::insert( light_colors )
            ( Qt::white,  Qt::white )
            ( Qt::red,    Qt::red   )
            ( Qt::green,  Qt::green )
            ( Qt::blue,   Qt::blue  )
            ( Qt::cyan,   Qt::cyan  )
            ( Qt::magenta,Qt::magenta )
            ( Qt::yellow, Qt::yellow  )
            ( Qt::gray,   Qt::gray  );
    }

    if( dark_colors.empty() )
    {
        boost::assign::insert( dark_colors )
            ( Qt::white,  Qt::black )
            ( Qt::red,    Qt::darkRed   )
            ( Qt::green,  Qt::darkGreen )
            ( Qt::blue,   Qt::darkBlue  )
            ( Qt::cyan,   Qt::darkCyan  )
            ( Qt::magenta,Qt::darkMagenta )
            ( Qt::yellow, Qt::darkYellow  )
            ( Qt::gray,   Qt::darkGray  );
    }

    if( background.lightness() > 128 )
        return dark_colors[standard];
    else
        return light_colors[standard];
}

QString get_longest_common_subsequence(const QString& a, const QString& b)
{
    // algorithm taken from WikiPedia )

    using namespace std;
    vector<vector<int> > max_len;
    max_len.resize(a.size() + 1);
    for (int i = 0; i <= (int) a.size(); i++)
        max_len[i].resize(b.size() + 1);
    for (int i = (int) a.size() - 1; i >= 0; i--)
    {
        for (int j = (int) b.size() - 1; j >= 0; j--)
        {
            if (a[i] == b[j])
            {
                max_len[i][j] = 1 + max_len[i + 1][j + 1];
            }
            else
            {
                max_len[i][j] = max(max_len[i + 1][j], max_len[i][j + 1]);
            }
        }
    }
    QString res;
    for (int i = 0, j = 0; max_len[i][j] != 0 && i < (int) a.size() && j < (int) b.size();)
    {
        if (a[i] == b[j])
        {
            res.push_back(a[i]);
            i++;
            j++;
        }
        else
        {
            if (max_len[i][j] == max_len[i + 1][j])
                i++;
            else
                j++;
        }
    }
    return res;
}

QString common_substring_from_begin(const QString& s1, const QString& s2)
{
    QString res;
    int sz = qMin(s1.length(), s2.length());
    for (int i = 0; i < sz; i++)
    {
        const QChar& ch1 = s1[i];
        const QChar& ch2 = s2[i];
        if (ch1 == ch2)
            res += ch1;
        else
            break;
    }
    return res;
}

QString common_substring_from_end(const QString& s1, const QString& s2)
{
    QString res;
    int i1 = s1.length() - 1;
    int i2 = s2.length() - 1;
    while (i1 > 0 && i2 > 0)
    {
        const QChar& ch1 = s1[i1];
        const QChar& ch2 = s2[i2];
        if (ch1 == ch2)
            res = ch1 + res;
        else
            break;
        i1--;
        i2--;
    }
    return res;
}


//#include <string>
//
//using std::string;
//
//
//int LongestCommonSubstring(const QString& str1, const QString& str2, QString& sequence)
//{
//	sequence = QString();
//	if (str1.isEmpty() || str2.isEmpty())
//		return 0;
//
//	int[,] num = new int[str1.Length, str2.Length];
//	int maxlen = 0;
//	int lastSubsBegin = 0;
//	StringBuilder sequenceBuilder = new StringBuilder();
//
//	for (int i = 0; i < str1.Length; i++)
//	{
//		for (int j = 0; j < str2.Length; j++)
//		{
//			if (str1[i] != str2[j])
//				num[i, j] = 0;
//			else
//			{
//				if ((i == 0) || (j == 0))
//					num[i, j] = 1;
//				else
//					num[i, j] = 1 + num[i - 1, j - 1];
//
//				if (num[i, j] > maxlen)
//				{
//					maxlen = num[i, j];
//					int thisSubsBegin = i - num[i, j] + 1;
//					if (lastSubsBegin == thisSubsBegin)
//					{//if the current LCS is the same as the last time this block ran
//						sequenceBuilder.Append(str1[i]);
//					}
//					else //this block resets the string builder if a different LCS is found
//					{
//						lastSubsBegin = thisSubsBegin;
//						sequenceBuilder.Remove(0, sequenceBuilder.Length);//clear it
//						sequenceBuilder.Append(str1.Substring(lastSubsBegin, (i + 1) - lastSubsBegin));
//					}
//				}
//			}
//		}
//	}
//	sequence = sequenceBuilder.ToString();
//	return maxlen;
//}


