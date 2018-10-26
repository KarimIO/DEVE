import React, { Component } from 'react';

export default class RequestList extends Component {
	render() {
		let emptyrequests = this.props.requests.length === 0;
		let emptyongoing = this.props.ongoing.length === 0;
		return (
			<section className={`main-section${this.props.loading ? " loading" : ""}`}>
                <h2>Pending Requests{emptyrequests && <i> (None Found)</i>}</h2>
                <div className="gallery">
                    {this.props.requests.map((e, i) => {
                        return <div key={i} style={{backgroundImage: "url(/img/"+e.url+")"}}>
                            <div>
                                <span>
                                    <b>{e.user}</b>
                                    <i>{e.views} view{e.views !== 1 && "s"} requested</i>
									<button>Grant Views</button>
                                </span>
                            </div>
                        </div>;
                    })}
                </div>
                <h2>Ongoing Requests{emptyongoing && <i> (None Found)</i>}</h2>
                <div className="gallery">
                    {this.props.ongoing.map((e, i) => {
                        return <div key={i} style={{backgroundImage: "url(/img/"+e.url+")"}}>
                            <div>
                                <span>
                                    <b>{e.user}</b>
                                    <i>{e.views} view{e.views !== 1 && "s"} left</i>
									<button>Grant More</button>
                                </span>
                            </div>
                        </div>;
                    })}
                </div>
			</section>
		);
	}
}