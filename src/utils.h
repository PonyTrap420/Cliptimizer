#include "stdafx.h"

#define STR(x) QString::number(x)

namespace util {
    QString MsToFormat(int ms, QString format) {
        return QDateTime::fromSecsSinceEpoch(ms / 1000).toUTC().toString(format);
    }

    QString GetExtention(QString path)
    {
        std::filesystem::path fPath = path.toStdString();
        return QString::fromStdString(fPath.extension().string());
    }

    int GetFileSize(QString path)
    {
        int size;
        QFile file(path);
        if (file.open(QIODevice::ReadOnly)) {
            size = file.size();
            file.close();
        }
        return size;
    }

    void SetButtonIcon(QPushButton* btn, QString name) {
        QIcon icon;
        icon.addFile((":/Cliptimizer/icons/"+name+".svg"), QSize(), QIcon::Normal, QIcon::Off);
        btn->setIcon(icon);
        btn->setToolTip(name[0].toUpper() + name.mid(1));
    }
}