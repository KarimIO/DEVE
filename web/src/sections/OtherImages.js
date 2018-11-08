import React, { Component } from 'react';

export default class MyImages extends Component {
	render() {
		let empty = this.props.gallery.length === 0;
		return (
			<section className={`main-section${this.props.loading ? " loading" : ""}${empty ? " empty":""}`}>
				<div className="gallery">
					{this.props.gallery.map((e, i) => {
						return <div key={i} style={{backgroundImage: "url(data:image/jpeg;base64,"+e.thumb+")"}}>
							<div>
								<span>
									{e.views > 0 && <i>{e.views} view{e.views !== 1 && "s"} left</i>}
									{e.views === 0 && <button>Request</button>}
									{e.views > 0 && <button>Request More</button>}
								</span>
							</div>
						</div>;
					})}
				</div>
			</section>
		);
	}
}