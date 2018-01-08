import { Component, OnDestroy, ViewChild } from '@angular/core';
import {EventService} from "../../../event.service";
import {AbstractObserverComponent} from "../abstract-observer.component";
import {CameraService} from "../../../camera.service";
import { RaceStripService } from "../../race-strip.service";
import { PreviewComponent } from "../../../preview/preview.component";

@Component({
    selector: 'finish-detection',
    template: require('./finish-detection.html'),
    styles: [require('./finish-detection.scss')]
})
export class FinishDetectionComponent extends AbstractObserverComponent implements OnDestroy {
    private filter: string = 'cvt';
    @ViewChild('prv') private previewComp: PreviewComponent;

    constructor(public cameraService: CameraService,
                public eventService: EventService) {
        super(eventService, cameraService, 'finish_detection')
    }

    ngOnDestroy() {
        setTimeout(() => {
            this.previewComp.ngOnDestroy();
        }, 200);
    }

    updateRoi() {
        if (this.observer) {
            this.observer.roi.x = 0;
            this.observer.roi.width = this.max_width;
            this.observer.roi.height = this.max_height - this.observer.roi.y;
            this.setRoi();
        }
    }

    updateMetadata(observer: any) {
    }

}