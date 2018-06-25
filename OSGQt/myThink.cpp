class PagedCullingCallback : public osg::NodeCallback
{
public:
    virtual void operator()( osg::Node* node, osg::NodeVisitor* nv )
    {
        osg::PagedLOD* pagedLOD = static_cast<osg::PagedLOD*>(node);
        if ( pagedLOD && pagedLOD->getNumChildren()>0 )
        {
            unsigned int numChildren = pagedLOD->getNumChildren();
            bool updateTimeStamp = nv->getVisitorType()==osg::NodeVisitor::CULL_VISITOR;
            if ( nv->getFrameStamp() && updateTimeStamp )
            {
                double timeStamp = nv->getFrameStamp()?nv->getFrameStamp()->getReferenceTime():0.0;
                unsigned int frameNumber = nv->getFrameStamp()?nv->getFrameStamp()->getFrameNumber():0;

                pagedLOD->setFrameNumberOfLastTraversal( frameNumber );
                pagedLOD->setTimeStamp( numChildren-1, timeStamp );
                pagedLOD->setFrameNumber( numChildren-1, frameNumber );
                pagedLOD->getChild(numChildren-1)->accept(*nv);
            }

            // Request for new child
            if ( !pagedLOD->getDisableExternalChildrenPaging() &&
                 nv->getDatabaseRequestHandler() &&
                 numChildren<pagedLOD->getNumRanges() )
            {
                if ( pagedLOD->getDatabasePath().empty() )
                {
                    nv->getDatabaseRequestHandler()->requestNodeFile(
                        pagedLOD->getFileName(numChildren), nv->getNodePath(),
                        1.0, nv->getFrameStamp(),
                        pagedLOD->getDatabaseRequest(numChildren), pagedLOD->getDatabaseOptions() );
                }
                else
                {
                    nv->getDatabaseRequestHandler()->requestNodeFile(
                        pagedLOD->getDatabasePath()+pagedLOD->getFileName(numChildren), nv->getNodePath(),
                        1.0, nv->getFrameStamp(),
                        pagedLOD->getDatabaseRequest(numChildren), pagedLOD->getDatabaseOptions() );
                }
            }
        }
    }
};
static osg::ref_ptr<PagedCullingCallback> g_pagedCullingCallback = new PagedCullingCallback;