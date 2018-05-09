MyWidget::MyWidget(QWidget *parent) :
 QWidget(parent)
{
  this->setFixedSize(400, 300);
  progress_value = 0;
  timer = new QTimer();
  timer->setInterval(1000);
  timer->start();
  progress_bar = new QProgressBar(this);
  progress_bar->setGeometry(50, 50, 150, 20);
  progress_bar->setValue(0);
  connect(timer, SIGNAL(timeout()), this, SLOT(changeValue()));
}
void MyWidget::changeValue()
{
  progress_value++;
  if(progress_value > 100)
  {
    timer->stop();
  }
  progress_bar->setValue(progress_value);
}