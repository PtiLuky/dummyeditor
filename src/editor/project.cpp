#include "editor/project.hpp"

#include <QDir>
#include <algorithm>

#include "utils/logger.hpp"
#include "widgets/mapsTree.hpp"

using std::make_shared;
using std::make_unique;

namespace Editor {

Project::Project(const QString& projectFolder)
{
    /*
      // Try to read the "project.xml" file that should be present in folderPath.
      QFile xmlProjectFile((m_projectPath / "project.xml").string().c_str());
      m_domDocument.setContent(&xmlProjectFile);
      xmlProjectFile.close();

      QDomNodeList mapsNodes = m_domDocument.documentElement().elementsByTagName("maps");

      if (mapsNodes.length() > 0) {
          m_mapsModel = make_unique<MapsTreeModel>(mapsNodes.at(0));
      } else {
          // XXX: Throw exception?
      }

      QDomNodeList startingPositionNodes = m_domDocument.documentElement().elementsByTagName("starting_point");

      if (startingPositionNodes.length() > 0) {
          const QDomNode& startingPositionNode(startingPositionNodes.at(0));
          const QDomNamedNodeMap& attributes(startingPositionNode.attributes());
          m_startingPoint = StartingPoint(attributes.namedItem("map").nodeValue().toStdString().c_str(),
                                          attributes.namedItem("x").nodeValue().toUShort(),
                                          attributes.namedItem("y").nodeValue().toUShort(),
                                          attributes.namedItem("floor").nodeValue().toUShort());
      }*/
    m_projectPath = projectFolder;
    m_mapsModel   = make_unique<MapsTreeModel>(m_game);
}

const QString& Project::projectPath() const
{
    return m_projectPath;
}

const Dummy::GameStatic& Project::game() const
{
    return m_game;
}

MapsTreeModel* Project::mapsModel() const
{
    return m_mapsModel.get();
}

const Dummy::Map* Project::currMap() const
{
    return m_currMap.get();
}

std::shared_ptr<Project> Project::create(const QString& projectRootPath)
{
    // Check folder exists and is empty
    const QDir projectDir(projectRootPath);
    if (! projectDir.exists()) {
        error(QObject::tr("Project directory does not exist: %1").arg(projectDir.absolutePath()));
        return nullptr;
    }
    if (! projectDir.isEmpty()) {
        error(QObject::tr("Project directory must be empty: %1").arg(projectDir.absolutePath()));
        return nullptr;
    }

    // Create subfolders
    projectDir.mkdir("maps");
    projectDir.mkdir("images");
    projectDir.mkdir("fonts");
    projectDir.mkdir("sounds");

    // Create DummyRPG Editor project file
    QFile projectFile(projectDir.filePath("dummy-rpg-project.xml"));
    projectFile.open(QIODevice::WriteOnly | QIODevice::Text);
    if (! projectFile.isOpen()) {
        error(QObject::tr("Could not create project file: %1").arg(projectFile.fileName()));
        return nullptr;
    }

    return std::make_shared<Project>(projectRootPath);
} /*

 QDomDocument Project::createXmlProjectTree()
 {
     QDomDocument doc;
     QDomElement project = doc.createElement("project");
     QDomElement maps    = doc.createElement("maps");

     doc.appendChild(project);
     project.appendChild(maps);

     return doc;
 }
 */
void Project::saveProject()
{
    /*
       QDomDocument doc;
       QDomElement projectNode = doc.createElement("project");
       QDomElement mapsNode    = doc.createElement("maps");

       doc.appendChild(projectNode);
       projectNode.appendChild(mapsNode);

       QDomElement startingPointNode = doc.createElement("starting_point");
       startingPointNode.setAttribute("map", m_startingPoint.mapName());
       startingPointNode.setAttribute("x", m_startingPoint.x());
       startingPointNode.setAttribute("y", m_startingPoint.y());
       startingPointNode.setAttribute("floor", static_cast<uint16_t>(m_startingPoint.floor()));
       projectNode.appendChild(startingPointNode);

       dumpToXmlNode(doc, mapsNode, m_mapsModel->invisibleRootItem());
       QString xmlPath((m_projectPath / "project.xml").string().c_str());

       // XXX: Handle errors eventually.
       QFile file(xmlPath);
       file.open(QIODevice::WriteOnly | QIODevice::Text);
       QTextStream stream(&file);
       const int indent = 4;
       doc.save(stream, indent);

       if (openedMaps().count() > 0) {
           for (auto e : openedMaps().keys()) {
               document(e).m_map->save();
           }
       }
  */
}
/*
void Project::dumpToXmlNode(QDomDocument& doc, QDomElement& xmlNode, const QStandardItem* modelItem)
{
    const int nbRows = modelItem->rowCount();
    for (int i = 0; i < nbRows; ++i) {
        const QStandardItem* mapItem = modelItem->child(i);

        QDomElement mapNode = doc.createElement("map");
        mapNode.setAttribute("name", mapItem->text());

        xmlNode.appendChild(mapNode);

        dumpToXmlNode(doc, mapNode, mapItem);
    }
}
*/

void Project::createMap(const tMapInfo& mapInfo, QStandardItem& parent)
{
    if (m_mapsModel == nullptr)
        return;

    const uint16_t w            = mapInfo.m_width;
    const uint16_t h            = mapInfo.m_height;
    const QString mapName       = QString::fromStdString(mapInfo.m_mapName);
    const Dummy::char_id chipId = m_game.registerChipset(mapInfo.m_chispetPath);

    m_currMap = make_shared<Dummy::Map>(w, h, chipId);

    // Add the new map into the tree.
    QList<QStandardItem*> mapRow {new QStandardItem(mapName)};
    parent.appendRow(mapRow);
}

bool Project::loadMap(const QString& mapName)
{
    // TODO
    return true;
}

} // namespace Editor
