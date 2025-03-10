// SPDX-FileCopyrightText: 2025 Thomas Duckworth <tduck@filotimoproject.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "editor.h"

#include <KAuth/Action>
#include <KAuth/ExecuteJob>
#include <KLocalizedString>
#include <KMessageWidget>
#include <KSeparator>
#include <KTextEditor/Document>
#include <KTextEditor/Editor>
#include <KTextEditor/View>

#include <QDialog>
#include <QDialogButtonBox>
#include <QLabel>
#include <QPointer>
#include <QPushButton>
#include <QStyle>
#include <QVBoxLayout>

Editor::Editor(QObject *parent)
    : QObject(parent)
{
}

void Editor::openEditor(const QString &file, QWindow *parentWindow)
{
    auto editorInstance = KTextEditor::Editor::instance();
    auto document = editorInstance->createDocument(this);

    // Using a QPointer is safer for modal dialogs.
    // See: https://blogs.kde.org/node/3919
    QPointer<QDialog> dlgEditor = new QDialog(nullptr);
    dlgEditor->setWindowTitle(i18n("Editing %1", file.section(QLatin1Char('/'), -1)));
    dlgEditor->setMinimumSize(800, 600);

    auto editorView = document->createView(dlgEditor);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel, dlgEditor);
    connect(buttonBox, SIGNAL(accepted()), dlgEditor, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), dlgEditor, SLOT(reject()));
    buttonBox->button(QDialogButtonBox::Save)->setEnabled(false);

    QLabel *lblFilePath = new QLabel(i18n("Editing file: <code>%1</code>", file));
    lblFilePath->setTextFormat(Qt::RichText);

    QVBoxLayout *vlayout = new QVBoxLayout(dlgEditor);
    vlayout->setContentsMargins(0, 0, 0, 0);
    vlayout->setSpacing(0);

    QVBoxLayout *lblFilepathLayout = new QVBoxLayout();
    lblFilepathLayout->addWidget(lblFilePath);
    lblFilepathLayout->setContentsMargins(6, 6, 6, 6);
    lblFilepathLayout->setSpacing(QStyle::PixelMetric::PM_LayoutVerticalSpacing);

    QVBoxLayout *buttonBoxLayout = new QVBoxLayout();
    buttonBoxLayout->addWidget(buttonBox);
    buttonBoxLayout->setContentsMargins(6, 6, 6, 6);
    buttonBoxLayout->setSpacing(QStyle::PixelMetric::PM_LayoutVerticalSpacing);

    vlayout->addLayout(lblFilepathLayout);
    vlayout->addWidget(new KSeparator());
    vlayout->addWidget(editorView);
    vlayout->addWidget(new KSeparator());
    vlayout->addLayout(buttonBoxLayout);

    // Read contents of unit file.
    if (!document->openUrl(QUrl::fromLocalFile(file))) {
        Q_EMIT errorOccurred(i18n("Failed to open the unit file:\n%1", file));
        return;
    }

    connect(document, &KTextEditor::Document::textChanged, [=] {
        buttonBox->button(QDialogButtonBox::Save)->setEnabled(true);
    });

    connect(dlgEditor.get(), &QDialog::accepted, this, [this, file, document]() {
        // Declare a QVariantMap with arguments for the helper.
        QVariantMap helperArgs;
        helperArgs[QStringLiteral("file")] = file;
        helperArgs[QStringLiteral("contents")] = document->text();

        // Create the action.
        KAuth::Action action(QStringLiteral("org.kde.kcontrol.systemdgenie.saveunitfile"));
        action.setHelperId(QStringLiteral("org.kde.kcontrol.systemdgenie"));
        action.setArguments(helperArgs);

        KAuth::ExecuteJob *job = action.execute();
        if (!job->exec()) {
            Q_EMIT errorOccurred(i18n("Unable to authenticate/execute the action: %1", job->error()));
        }
    });

    dlgEditor->winId();
    auto handle = dlgEditor->windowHandle();
    handle->setTransientParent(parentWindow);
    dlgEditor->show();
}

#include "moc_editor.cpp"
