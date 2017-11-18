
import {Component, Input, ViewChild} from "@angular/core";
import {TrafficLightObserverComponent} from "../../../observers/traffic-light/traffic-light.component";
import {LaneDetectionComponent} from "../../../observers/lane-detection/lane-detection.component";
import {FinishDetectionComponent} from "../../../observers/finish-detection/finish-detection.component";
import {EventService, ROI_CLEAR} from "../../../event.service";

@Component({
    selector: 'drag-race',
    template: require('./drag-race.html')
})
export class DragRaceComponent {
    @ViewChild('traffic') traffic: TrafficLightObserverComponent;
    @ViewChild('lane') lane: LaneDetectionComponent;
    @ViewChild('finish') finish: FinishDetectionComponent;
    @Input('toggle')
    set toggle(start: boolean) {
        console.log('Start ', start);
        if (start) {
            this.traffic.activate();
        } else {
            this.traffic.deactivate();
            this.lane.deactivate();
            this.finish.deactivate();
        }
    }

    constructor(private eventService: EventService) {
        eventService.emit(ROI_CLEAR);
    }

    ngAfterViewInit() {
        this.traffic.deactivate();
        this.lane.deactivate();
        this.finish.deactivate();
    }

    ngOnDestroy() {
        this.traffic.deactivate();
        this.lane.deactivate();
        this.finish.deactivate();
    }
}