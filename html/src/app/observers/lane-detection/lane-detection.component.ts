
import { Component } from '@angular/core';
import { CarObserver, ObserverService } from '../observer.service';
@Component({
    selector: 'lane-detection',
    template: require('./lane-detection.html'),
    styles: [require('./lane-detection.scss')]
})
export class LaneDetectionComponent {
    private observer: CarObserver;

    constructor(private observerService: ObserverService) {}

    ngAfterViewInit() {
        this.observerService.getObserver('lane_detection').subscribe(data => {
            this.observer = data;
            setTimeout(() => 500);
        });
    }

    private updateObserver() {
        this.observer.active =  (Number(this.observer.active));
        this.observerService.saveObserver('lane_detection', this.observer).subscribe(observer => this.observer = observer);
    }

}