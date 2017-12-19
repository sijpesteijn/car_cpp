import {Component} from '@angular/core';
import {EventService} from "../../../event.service";
import {AbstractObserverComponent} from "../abstract-observer.component";
import {CameraService} from "../../../camera.service";

@Component({
    selector: 'finish-detection',
    template: require('./finish-detection.html'),
    styles: [require('./finish-detection.scss')]
})
export class FinishDetectionComponent extends AbstractObserverComponent {

    constructor(public cameraService: CameraService,
                public eventService: EventService) {
        super(eventService, cameraService, 'finish_detection')
    }

    updateRoi() {
        if (this.observer) {
            this.observer.roi.x = 0;
            this.observer.roi.width = this.max_width;
            this.observer.roi.height = this.max_height - this.observer.roi.y;
            this.setRoi();
        }
    }
}